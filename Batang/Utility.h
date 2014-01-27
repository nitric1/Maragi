#pragma once

namespace Batang
{
    std::wstring &trim(std::wstring &);
    std::vector<std::wstring> split(const std::wstring &, const std::wstring &);
    std::vector<std::wstring> splitAnyOf(const std::wstring &, const std::wstring &);
    std::wstring join(const std::vector<std::wstring> &, const std::wstring &);
    std::wstring getDirectoryPath(const std::wstring &);
    std::string encodeUtf8(const std::wstring &);
    std::wstring decodeUtf8(const std::string &);
    std::u32string encodeUtf32(const std::wstring &);
    std::string base64Encode(const std::vector<uint8_t> &);
    std::vector<uint8_t> base64Decode(const std::string &);
    std::string encodeUrl(const std::string &);
    std::string encodeUrlParam(const std::string &);
    std::string decodeUrl(const std::string &);

    float round(float);
    double round(double);
}
