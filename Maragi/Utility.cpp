#include "Defaults.h"

#include "Utility.h"

namespace Maragi
{
	std::string base64Encode(const std::vector<uint8_t> &data)
	{
		static const char hashChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static const char padding = '=';

		std::string str;
		str.reserve((data.size() + 2) * 4 / 3);
		uint8_t buf[3];
		char cand[4];
		size_t i = 0;

		for(auto it = data.begin(), ed = data.end(); it != ed; ++ it)
		{
			buf[i ++] = *it;
			if(i >= 3)
			{
				cand[0] = hashChar[(buf[0] & 0xFC) >> 2];
				cand[1] = hashChar[((buf[0] & 0x03) << 4) | ((buf[0] & 0xF0) >> 4)];
				cand[2] = hashChar[((buf[1] & 0x0F) << 2) | ((buf[2] & 0xC0) >> 6)];
				cand[3] = hashChar[buf[2] & 0x3F];
				str.insert(str.end(), cand, cand + 4);
				i = 0;
			}
		}

		if(i > 0)
		{
			size_t j;
			for(j = i; j < 3; ++ j)
				buf[j] = 0;
			cand[0] = hashChar[(buf[0] & 0xFC) >> 2];
			cand[1] = hashChar[((buf[0] & 0x03) << 4) | ((buf[0] & 0xF0) >> 4)];
			cand[2] = hashChar[((buf[1] & 0x0F) << 2) | ((buf[2] & 0xC0) >> 6)];
			cand[3] = hashChar[buf[2] & 0x3F];
			str.insert(str.end(), cand, cand + (i + 1));
			while(++ i <= 3)
				str.push_back('=');
		}
	}

	namespace
	{
		inline bool isBase64(char ch)
		{
			return isalnum(ch) || ch == '+' || ch == '/';
		}
	}

	//std::vector<uint8_t> base64Decode(const std::string &str)
	//{
	//}
}
