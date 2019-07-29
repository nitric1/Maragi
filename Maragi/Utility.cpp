#include "Common.h"

#include "../Batang/Utility.h"

#include "Constants.h"
#include "Utility.h"

namespace Maragi
{
    std::wstring getCurrentDirectoryPath()
    {
        std::array<wchar_t, 512> buffer;
        size_t pathlen;

        pathlen = GetModuleFileNameW(nullptr, buffer.data(), static_cast<ulong32_t>(buffer.size()));

        std::wstring path(buffer.begin(), buffer.begin() + pathlen);
        return Batang::getDirectoryPath(path);
    }

    std::wstring getDataDirectoryPath()
    {
        std::array<wchar_t, 512> buffer;
        size_t pathlen;

        pathlen = GetEnvironmentVariableW(L"AppData", buffer.data(), static_cast<ulong32_t>(buffer.size()));
        
        std::wstring path(buffer.begin(), buffer.begin() + pathlen);
        if(path.back() != L'\\')
            path.push_back(L'\\');

        path += Constants::TITLE;
        path += L"\\";

        CreateDirectoryW(path.c_str(), nullptr);

        return path;
    }
}
