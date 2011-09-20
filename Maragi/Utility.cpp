#include "Defaults.h"

#include "Utility.h"

#include "Constants.h"

namespace Maragi
{
	std::wstring &trim(std::wstring &str)
	{
		static auto fn = [](wchar_t ch) { return ch != L' ' && ch != L'¡¡' && ch != L'\t' && ch != L'\n' && ch != L'\r'; };
		str.erase(str.begin(), find_if(str.begin(), str.end(), fn));
		str.erase(find_if(str.rbegin(), str.rend(), fn).base(), str.end());
		return str;
	}

	std::vector<std::wstring> split(const std::wstring &str, const std::wstring &sep)
	{
		size_t ppos, pos = static_cast<size_t>(-static_cast<ptrdiff_t>(sep.size()));
		std::vector<std::wstring> ve;

		while(true)
		{
			ppos = pos + sep.size();
			pos = str.find(sep, ppos);

			if(pos == std::wstring::npos)
			{
				ve.push_back(str.substr(ppos));
				break;
			}
			else
				ve.push_back(str.substr(ppos, pos - ppos));
		}

		return ve;
	}

	std::vector<std::wstring> splitAnyOf(const std::wstring &str, const std::wstring &sep)
	{
		size_t ppos, pos = static_cast<size_t>(-1);
		std::vector<std::wstring> ve;

		while(true)
		{
			ppos = pos;
			pos = str.find_first_of(sep, ++ ppos);

			if(pos == std::wstring::npos)
			{
				ve.push_back(str.substr(ppos));
				break;
			}
			else
				ve.push_back(str.substr(ppos, pos - ppos));
		}

		return ve;
	}

	std::wstring join(const std::vector<std::wstring> &ve, const std::wstring &sep)
	{
		if(ve.empty())
			return std::wstring();

		std::wstring str = ve.front();
		for(auto it = ++ ve.begin(); it != ve.end(); ++ it)
		{
			str += sep;
			str += *it;
		}

		return str;
	}

	std::wstring getDirectoryPath(const std::wstring &ipath)
	{
		std::wstring path = ipath;
		size_t pos = path.rfind(L'\\');
		if(pos != std::wstring::npos)
			path.erase(++ pos);
		return path;
	}

	std::wstring getCurrentDirectoryPath()
	{
		std::vector<wchar_t> buffer(512);
		size_t pathlen;

		pathlen = GetModuleFileNameW(nullptr, &*buffer.begin(), static_cast<ulong32_t>(buffer.size()));

		std::wstring path(buffer.begin(), buffer.begin() + pathlen);
		return getDirectoryPath(path);
	}

	std::wstring getDataDirectoryPath()
	{
		std::vector<wchar_t> buffer(512);
		size_t pathlen;

		pathlen = GetEnvironmentVariableW(L"AppData", &*buffer.begin(), static_cast<ulong32_t>(buffer.size()));
		
		std::wstring path(buffer.begin(), buffer.begin() + pathlen);
		if(path.back() != L'\\')
			path.push_back(L'\\');

		path += Constants::TITLE;
		path += L"\\";

		CreateDirectoryW(path.c_str(), nullptr);

		return path;
	}

	std::string encodeUTF8(const std::wstring &str)
	{
		std::vector<char> result(static_cast<size_t>(WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr)) + 1);
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &*result.begin(), static_cast<int>(result.size()), nullptr, nullptr);
		return &*result.begin();
	}

	std::wstring decodeUTF8(const std::string &str)
	{
		std::vector<wchar_t> result(static_cast<size_t>(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0)) + 1);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &*result.begin(), static_cast<int>(result.size()));
		return &*result.begin();
	}

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

		return str;
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
