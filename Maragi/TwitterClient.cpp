#include "Common.h"

#include "Batang/Singleton.h"
#include "Batang/Utility.h"

#include "Gurigi/Dialog.h"

#include "Configure.h"
#include "Constants.h"
#include "Tokens.h"
#include "TwitterClient.h"

namespace Maragi
{
    TwitterClientError::TwitterClientError(const char *message) noexcept
        : std::runtime_error(message)
    {
    }

    TwitterClientError::TwitterClientError(const std::string &message) noexcept
        : std::runtime_error(message)
    {
    }

    TwitterClientError::TwitterClientError(const TwitterClientError &obj) noexcept
        : std::runtime_error(obj)
    {
    }

    TwitterClientError::~TwitterClientError() noexcept
    {
    }

    NotAuthorizedError::NotAuthorizedError(const char *message) noexcept
        : TwitterClientError(message)
    {
    }

    NotAuthorizedError::NotAuthorizedError(const std::string &message) noexcept
        : TwitterClientError(message)
    {
    }

    NotAuthorizedError::NotAuthorizedError(const NotAuthorizedError &obj) noexcept
        : TwitterClientError(obj)
    {
    }

    NotAuthorizedError::~NotAuthorizedError() noexcept
    {
    }

    TwitterClient::TwitterClient()
        : TwitterClient({}, {}, {})
    {}

    TwitterClient::TwitterClient(const std::string &iscreenName, const std::string &iaccessToken, const std::string &iaccessTokenSecret)
        : screenName_(iscreenName), accessToken_(iaccessToken), accessTokenSecret_(iaccessTokenSecret)
    {
        struct CurlInitializer : Batang::Singleton<CurlInitializer>
        {
            CurlInitializer()
            {
                if(curl_global_init(CURL_GLOBAL_ALL) != 0)
                {
                    // TODO: error
                }
            }
            ~CurlInitializer() { curl_global_cleanup(); }
        };

        CurlInitializer::instance();

        curl_ = curl_easy_init();
        if(curl_ == nullptr)
            throw(std::runtime_error("CURL initialization failed."));

        cbd_.client = this;

        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1l);
        curl_easy_setopt(curl_, CURLOPT_USERAGENT, Batang::encodeUtf8(Constants::USER_AGENT).c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, static_cast<void *>(&cbd_));

        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0l);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0l);
    }

    TwitterClient::~TwitterClient()
    {
        curl_easy_cleanup(curl_);
    }

    size_t TwitterClient::curlWriteCallback(void *data, size_t size, size_t nmemb, void *param)
    {
        CurlWriteCallbackData *cbd_ = static_cast<CurlWriteCallbackData *>(param);
        size_t realSize = size * nmemb;

        cbd_->data.insert(cbd_->data.end(), static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + realSize);

        if(cbd_->cb)
            cbd_->cb(cbd_->data.size());

        return realSize;
    }

    namespace
    {
        std::string join(const std::vector<std::string> &ve, const std::string &sep)
        {
            if(ve.empty())
                return std::string();

            std::string str = ve.front();
            for(auto it = ++ ve.begin(); it != ve.end(); ++ it)
            {
                str += sep;
                str += *it;
            }

            return str;
        }

        Url makeRequestUrl(const std::string &path)
        {
            std::string uri = Paths::PROTOCOL;
            uri += "://";
            uri += Paths::HOST;
            uri += "/";
            uri += path;

            return Url(uri);
        }

        class CryptProvider : public Batang::Singleton<CryptProvider>
        {
        private:
            HCRYPTPROV cp;

        public:
            CryptProvider(const wchar_t *containerName = nullptr)
            {
                if(!CryptAcquireContextW(&cp, containerName, nullptr, PROV_RSA_FULL, 0))
                {
                    if(GetLastError() == NTE_BAD_KEYSET)
                    {
                        if(!CryptAcquireContextW(&cp, containerName, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET))
                            throw(std::runtime_error("Cannot initialize pseudo-random number generator."));
                    }
                    else
                        throw(std::runtime_error("Cannot initialize pseudo-random number generator."));
                }
            }

            ~CryptProvider()
            {
                CryptReleaseContext(cp, 0);
            }

            HCRYPTPROV get()
            {
                return cp;
            }
        };

        std::string toBase(unsigned long long num, int radix)
        {
            static const char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            std::string str;
            do
            {
                str += digit[num % radix];
                num /= radix;
            }
            while(num != 0);
            return str;
        }

        std::string generateNonce()
        {
            static CryptProvider cp(L"MaragiPRNG");
            unsigned long long rnd = 0;
            if(!CryptGenRandom(cp.get(), sizeof(rnd), reinterpret_cast<uint8_t *>(&rnd)))
                throw(std::runtime_error("Cannot generate pseudo-random number."));
            return toBase(rnd, 62);
        }

        std::string HmacSha1(const std::vector<uint8_t> &key, const std::vector<uint8_t> &message)
        {
            std::vector<uint8_t> normKey(20);

            if(key.size() > 64)
                SHA1(&*key.begin(), key.size(), &*normKey.begin());
            else
                normKey = key;

            std::vector<uint8_t> ipad = normKey, opad = normKey;
            ipad.resize(64); opad.resize(64);

            for(size_t i = 0; i < 64; ++ i)
            {
                ipad[i] ^= 0x36;
                opad[i] ^= 0x5C;
            }

            std::vector<uint8_t> result(20);

            ipad.insert(ipad.end(), message.begin(), message.end());
            SHA1(&*ipad.begin(), ipad.size(), &*result.begin());

            opad.insert(opad.end(), result.begin(), result.end());
            SHA1(&*opad.begin(), opad.size(), &*result.begin());

            return Batang::base64Encode(result);
        }

        std::string makePostField(const std::map<std::string, std::string> &field)
        {
            std::string text;
            if(!field.empty())
            {
                auto it = field.begin();
                text += Batang::encodeUrlParam(it->first);
                text += "=";
                text += Batang::encodeUrlParam(it->second);
                for(++ it; it != field.end(); ++ it)
                {
                    text += "&";
                    text += Batang::encodeUrlParam(it->first);
                    text += "=";
                    text += Batang::encodeUrlParam(it->second);
                }
            }

            return text;
        }

        std::string makeOAuthHeader(const std::map<std::string, std::string> &field)
        {
            std::string text = "Authorization: OAuth ";
            if(!field.empty())
            {
                auto it = field.begin();
                text += Batang::encodeUrl(it->first);
                text += "=\"";
                text += Batang::encodeUrl(it->second);
                text += "\"";
                for(++ it; it != field.end(); ++ it)
                {
                    text += ", ";
                    text += Batang::encodeUrl(it->first);
                    text += "=\"";
                    text += Batang::encodeUrl(it->second);
                    text += "\"";
                }
            }

            return text;
        }

        std::string signRequestUrl(Url &uri, const std::string &tokenSecret = std::string())
        {
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            unsigned long long ts = (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
            ts -= 116444736000000000;
            ts /= 10000000;

            std::stringstream s;
            s << ts;

            std::string nonce = generateNonce();

            uri.addOAuthParam("oauth_consumer_key", AppTokens::CONSUMER_KEY);
            uri.addOAuthParam("oauth_version", "1.0");
            uri.addOAuthParam("oauth_timestamp", s.str());
            uri.addOAuthParam("oauth_nonce", nonce);
            uri.addOAuthParam("oauth_signature_method", "HMAC-SHA1");
            uri.removeOAuthParam("oauth_signature");

            std::string message = "POST&";
            message += Batang::encodeUrl(uri.baseUrl());
            message += "&";
            std::map<std::string, std::string> totalParams = uri.params();
            const auto &oauthParams = uri.oauthParams();
            totalParams.insert(oauthParams.begin(), oauthParams.end());
            message += Batang::encodeUrl(makePostField(totalParams));

            std::string key = Batang::encodeUrl(AppTokens::CONSUMER_SECRET);
            key += "&";
            key += Batang::encodeUrl(tokenSecret);

            uri.addOAuthParam(
                "oauth_signature",
                HmacSha1(
                    std::vector<uint8_t>(key.begin(), key.end()),
                    std::vector<uint8_t>(message.begin(), message.end())
                )
            );

            return nonce;
        }

        template<typename It>
        std::map<std::string, std::string> parsePostField(It begin, It end)
        {
            std::map<std::string, std::string> field;
            std::string key, value;
            bool inValue = false;
            for(It &it = begin; it != end; ++ it)
            {
                if(*it == '&')
                {
                    field.insert(std::make_pair(Batang::decodeUrl(key), Batang::decodeUrl(value)));
                    key.clear(); value.clear();
                    inValue = false;
                }
                else if(*it == '=' && !inValue)
                    inValue = true;
                else if(inValue)
                    value += *it;
                else
                    key += *it;
            }
            return field;
        }

        // TODO: Separate the dialog into whole complete class and file.
        class ConfirmDialog : public Gurigi::Dialog, public Batang::Singleton<ConfirmDialog>
        {
        private:
            ConfirmDialog() : Dialog(nullptr) {}

        private:
            std::wstring text_;

        public:
            virtual const wchar_t *dialogName() override
            {
                return MAKEINTRESOURCEW(IDD_CONFIRM);
            }

            const std::wstring &text()
            {
                return text_;
            }

            bool show()
            {
                return showModal(Batang::delegate(this, &ConfirmDialog::procMessage)) == IDOK;
            }

            virtual intptr_t procMessage(HWND window, unsigned message, WPARAM wParam, LPARAM lParam)
            {
                ConfirmDialog &self = instance();

                switch(message)
                {
                case WM_INITDIALOG:
                    {
                        HWND text = GetDlgItem(window, IDC_TEXT);

                        HFONT font = reinterpret_cast<HFONT>(SendMessageW(text, WM_GETFONT, 0, 0));
                        LOGFONTW lf;
                        GetObjectW(font, sizeof(lf), &lf);

                        lf.lfWeight = FW_BOLD;
                        lf.lfHeight *= 5;

                        font = CreateFontIndirectW(&lf);

                        SendMessageW(text, WM_SETFONT, reinterpret_cast<WPARAM>(font), 1);

                        SetFocus(text);
                    }
                    return 1;

                case WM_COMMAND:
                    switch(LOWORD(wParam))
                    {
                    case IDOK:
                        {
                            HWND text = GetDlgItem(window, IDC_TEXT);
                            int len = GetWindowTextLengthW(text) + 1;
                            std::vector<wchar_t> buf(static_cast<size_t>(len));
                            GetWindowTextW(text, &*buf.begin(), len);
                            self.text_ = &*buf.begin();
                            self.endDialog(IDOK);
                        }
                        return 1;

                    case IDCANCEL:
                        self.endDialog(IDCANCEL);
                        return 1;
                    }
                    break;

                case WM_CLOSE:
                    self.endDialog(IDCANCEL);
                    return 1;
                }

                return 0;
            }

            friend class Batang::Singleton<ConfirmDialog>;
        };
    }

    void TwitterClient::authorize()
    {
        post([this]()
        {
            ConfirmDialog &cfd = ConfirmDialog::instance();

            Url uri = makeRequestUrl(Paths::REQUEST_TOKEN);
            uri.addOAuthParam("oauth_callback", "oob");
            signRequestUrl(uri);
            sendRequest(uri);

            std::map<std::string, std::string> recvParams = parsePostField(cbd_.data.begin(), cbd_.data.end());
            std::string token = recvParams["oauth_token"], tokenSecret = recvParams["oauth_token_secret"];

            uri = makeRequestUrl(Paths::AUTHORIZE);
            uri.addParam("oauth_token", token);

            std::wstring wuri = Batang::decodeUtf8(uri);
            ShellExecuteW(nullptr, nullptr, wuri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

            cfd.show();

            uri = makeRequestUrl(Paths::ACCESS_TOKEN);
            uri.addOAuthParam("oauth_token", token);
            uri.addOAuthParam("oauth_verifier", Batang::encodeUtf8(cfd.text()));
            signRequestUrl(uri, tokenSecret);
            sendRequest(uri);

            std::string fields(cbd_.data.begin(), cbd_.data.end());
            MessageBoxA(nullptr, fields.c_str(), "Access Token", MB_OK);
        });
    }

    bool TwitterClient::authorized() const
    {
        return !accessToken_.empty() && !accessTokenSecret_.empty();
    }

    const std::string &TwitterClient::screenName() const
    {
        return screenName_;
    }

    const std::string &TwitterClient::accessToken() const
    {
        return accessToken_;
    }

    const std::string &TwitterClient::accessTokenSecret() const
    {
        return accessTokenSecret_;
    }

    void TwitterClient::run()
    {
        // TODO: twitter stream
    }

    bool TwitterClient::sendRequest(const Url &uri)
    {
        curl_easy_setopt(curl_, CURLOPT_URL, uri.baseUrl().c_str());
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, makePostField(uri.params()).c_str());

        curl_slist *header = curl_slist_append(nullptr, makeOAuthHeader(uri.oauthParams()).c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);

        cbd_.data.clear();

        CURLcode res = curl_easy_perform(curl_);
        curl_slist_free_all(header);

        return res == CURLE_OK;
    }
}
