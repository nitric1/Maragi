// $Id$

#include "Defaults.h"

#include "TwitterClient.h"

#include "URI.h"
#include "Constants.h"
#include "Utility.h"

namespace Maragi
{
	TwitterClient::TwitterClient()
	{
		initializeCurl();

		curl = curl_easy_init();
		if(curl == nullptr)
			throw(std::runtime_error("CURL initialization failed."));

		cbd.client = this;

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1l);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, encodeUTF8(Constants::USER_AGENT).c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&cbd));

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0l);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0l);
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

		void signRequestURI(URI &uri)
		{
		}

		std::string makePostField(const std::map<std::string, std::string> &ve)
		{
			return std::string();
		}

		std::vector<uint8_t> HMACSHA1(const std::vector<uint8_t> &key, const std::vector<uint8_t> &message)
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

			return result;
		}
	}

	void TwitterClient::authorize()
	{
		;
	}
}
