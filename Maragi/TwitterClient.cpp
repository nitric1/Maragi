#include "Common.h"

#include "Batang/Utility.h"

#include "Configure.h"
#include "Constants.h"
#include "Dialog.h"
#include "Batang/Singleton.h"
#include "Tokens.h"
#include "TwitterClient.h"

namespace Maragi
{
    TwitterClientError::TwitterClientError(const char *message) throw()
        : std::runtime_error(message)
    {
    }

    TwitterClientError::TwitterClientError(const std::string &message) throw()
        : std::runtime_error(message)
    {
    }

    TwitterClientError::TwitterClientError(const TwitterClientError &obj) throw()
        : std::runtime_error(obj)
    {
    }

    TwitterClientError::~TwitterClientError() throw()
    {
    }

    NotAuthorizedError::NotAuthorizedError(const char *message) throw()
        : TwitterClientError(message)
    {
    }

    NotAuthorizedError::NotAuthorizedError(const std::string &message) throw()
        : TwitterClientError(message)
    {
    }

    NotAuthorizedError::NotAuthorizedError(const NotAuthorizedError &obj) throw()
        : TwitterClientError(obj)
    {
    }

    NotAuthorizedError::~NotAuthorizedError() throw()
    {
    }

    TwitterClient::TwitterClient()
    {
        initializeCurl();

        curl = curl_easy_init();
        if(curl == nullptr)
            throw(std::runtime_error("CURL initialization failed."));

        cbd.client = this;

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1l);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, Batang::encodeUTF8(Constants::USER_AGENT).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&cbd));

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0l);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0l);
    }

    TwitterClient::TwitterClient(const std::string &iscreenName, const std::string &iaccessToken, const std::string &iaccessTokenSecret)
        : screenName(iscreenName), accessToken(iaccessToken), accessTokenSecret(iaccessTokenSecret)
    {
    }

    TwitterClient::TwitterClient(const TwitterClient &)
    {
    }

    TwitterClient::~TwitterClient()
    {
        curl_easy_cleanup(curl);
    }

    bool TwitterClient::initializeCurl()
    {
        static bool initialized = false;

        if(!initialized && curl_global_init(CURL_GLOBAL_ALL) == 0)
            initialized = true;

        return initialized;
    }

    size_t TwitterClient::curlWriteCallback(void *data, size_t size, size_t nmemb, void *param)
    {
        CurlWriteCallbackData *cbd = static_cast<CurlWriteCallbackData *>(param);
        size_t realSize = size * nmemb;

        cbd->data.insert(cbd->data.end(), static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + realSize);

        // TODO: Call callback function
        if(cbd->cb)
            cbd->cb(cbd->data.size());

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

        URI makeRequestURI(const std::string &path)
        {
            std::string uri = Paths::PROTOCOL;
            uri += "://";
            uri += Paths::HOST;
            uri += "/";
            uri += path;

            return URI(uri);
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

        std::string HMACSHA1(const std::vector<uint8_t> &key, const std::vector<uint8_t> &message)
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
                text += Batang::encodeURIParam(it->first);
                text += "=";
                text += Batang::encodeURIParam(it->second);
                for(++ it; it != field.end(); ++ it)
                {
                    text += "&";
                    text += Batang::encodeURIParam(it->first);
                    text += "=";
                    text += Batang::encodeURIParam(it->second);
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
                text += Batang::encodeURI(it->first);
                text += "=\"";
                text += Batang::encodeURI(it->second);
                text += "\"";
                for(++ it; it != field.end(); ++ it)
                {
                    text += ", ";
                    text += Batang::encodeURI(it->first);
                    text += "=\"";
                    text += Batang::encodeURI(it->second);
                    text += "\"";
                }
            }

            return text;
        }

        std::string signRequestURI(URI &uri, const std::string &tokenSecret = std::string())
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
            message += Batang::encodeURI(uri.getBaseURI());
            message += "&";
            std::map<std::string, std::string> totalParams = uri.getParams();
            const auto &oauthParams = uri.getOAuthParams();
            totalParams.insert(oauthParams.begin(), oauthParams.end());
            message += Batang::encodeURI(makePostField(totalParams));

            std::string key = Batang::encodeURI(AppTokens::CONSUMER_SECRET);
            key += "&";
            key += Batang::encodeURI(tokenSecret);

            uri.addOAuthParam(
                "oauth_signature",
                HMACSHA1(
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
                    field.insert(std::make_pair(Batang::decodeURI(key), Batang::decodeURI(value)));
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
        class ConfirmDialog : public UI::Dialog, public Batang::Singleton<ConfirmDialog>
        {
        private:
            ConfirmDialog() : Dialog(nullptr) {}

        private:
            std::wstring text;

        public:
            virtual const wchar_t *getDialogName()
            {
                return MAKEINTRESOURCEW(IDD_CONFIRM);
            }

            const std::wstring &getText()
            {
                return text;
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
                            self.text = &*buf.begin();
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

    bool TwitterClient::authorize()
    {
        ConfirmDialog &cfd = ConfirmDialog::inst();

        URI uri = makeRequestURI(Paths::REQUEST_TOKEN);
        uri.addOAuthParam("oauth_callback", "oob");
        signRequestURI(uri);
        sendRequest(uri);

        std::map<std::string, std::string> recvParams = parsePostField(cbd.data.begin(), cbd.data.end());
        std::string token = recvParams["oauth_token"], tokenSecret = recvParams["oauth_token_secret"];

        uri = makeRequestURI(Paths::AUTHORIZE);
        uri.addParam("oauth_token", token);

        std::wstring wuri = Batang::decodeUTF8(uri);
        ShellExecuteW(nullptr, nullptr, wuri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

        cfd.show();

        uri = makeRequestURI(Paths::ACCESS_TOKEN);
        uri.addOAuthParam("oauth_token", token);
        uri.addOAuthParam("oauth_verifier", Batang::encodeUTF8(cfd.getText()));
        signRequestURI(uri, tokenSecret);
        sendRequest(uri);
        
        std::string fields(cbd.data.begin(), cbd.data.end());
        MessageBoxA(nullptr, fields.c_str(), "Access Token", MB_OK);

        return true;
    }

    const std::string &TwitterClient::getScreenName()
    {
        return screenName;
    }

    const std::string &TwitterClient::getAccessToken()
    {
        return accessToken;
    }

    const std::string &TwitterClient::getAccessTokenSecret()
    {
        return accessTokenSecret;
    }

    bool TwitterClient::sendRequest(const URI &uri)
    {
        curl_easy_setopt(curl, CURLOPT_URL, uri.getBaseURI().c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, makePostField(uri.getParams()).c_str());

        curl_slist *header = curl_slist_append(nullptr, makeOAuthHeader(uri.getOAuthParams()).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        cbd.data.clear();

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(header);

        return res == CURLE_OK;
    }
}
