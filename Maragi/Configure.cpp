#include "Common.h"

#include "Utility.h"
#include "Configure.h"

namespace Maragi
{
    const std::wstring Configure::confFileName = L"Maragi.conf";

    Configure::Configure()
        : changed(false)
    {
        HANDLE file = CreateFileW(getConfigurePath().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file == INVALID_HANDLE_VALUE)
            return;

        size_t fileSize = GetFileSize(file, nullptr);

        std::vector<wchar_t> buffer(fileSize / sizeof(wchar_t));
        if(!ReadFile(file, &*buffer.begin(), static_cast<ulong32_t>(fileSize), nullptr, nullptr))
            return;
        buffer.push_back(L'\0');
        auto it = buffer.begin();
        if(*it == L'\xFEFF')
            ++ it;

        std::wstring str(&*it), name, value;
        std::wistringstream is(str);
        size_t pos;

        while(!is.eof())
        {
            getline(is, str);

            pos = str.find(L'=');
            if(pos == std::wstring::npos)
                continue;

            name.assign(str.begin(), str.begin() + pos);
            value.assign(str.begin() + pos + 1, str.end());

            trim(name);
            trim(value);

            set(name, value, false);
        }
    }

    Configure::~Configure()
    {
        if(!changed)
            return;

        std::wstring str(L"\xFEFF");
        for(auto it = confMap.begin(); it != confMap.end(); ++ it)
        {
            str += it->first;
            str += L"=";
            str += it->second;
            str += L"\n";
        }

        HANDLE file = CreateFileW(getConfigurePath().c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file == INVALID_HANDLE_VALUE)
            return;

        WriteFile(file, str.c_str(), static_cast<ulong32_t>(str.size() * sizeof(wchar_t)), nullptr, nullptr);
    }

    std::wstring Configure::getConfigurePath()
    {
        return getDataDirectoryPath() + confFileName;
    }

    namespace
    {
        inline uint8_t hexDigit(wchar_t ch)
        {
            if(ch >= L'0' && ch <= L'9')
                return static_cast<uint8_t>(ch - L'0');
            else if(ch >= L'A' && ch <= L'F')
                return static_cast<uint8_t>(ch - L'A' + 10);
            else if(ch >= L'a' && ch <= L'f')
                return static_cast<uint8_t>(ch - L'a' + 10);
            return 0xFF;
        }

        inline wchar_t digitHex(uint8_t low)
        {
            static wchar_t digits[16] =
            {
                L'0', L'1', L'2', L'3', L'4', L'5',
                L'6', L'7', L'8', L'9', L'A', L'B',
                L'C', L'D', L'E', L'F'
            };
            return digits[low];
        }
    }

    std::map<std::wstring, std::wstring>::iterator Configure::find(const std::wstring &name)
    {
        return confMap.find(name);
    }

    std::map<std::wstring, std::wstring>::const_iterator Configure::find(const std::wstring &name) const
    {
        return confMap.find(name);
    }

    bool Configure::exists(const std::wstring &name) const
    {
        return find(name) != confMap.end();
    }

    std::wstring Configure::get(const std::wstring &name, const std::wstring &def) const
    {
        auto it = find(name);
        if(it == confMap.end())
            return def;
        return it->second;
    }

    std::vector<uint8_t> Configure::getBinary(const std::wstring &name) const
    {
        std::wstring str = get(name);
        std::vector<uint8_t> ve;
        uint8_t v = 0;
        bool next = false;

        ve.reserve(str.size() / 2);

        for(auto it = str.begin(); it != str.end(); ++ it)
        {
            if(next)
            {
                v <<= 4;
                v |= hexDigit(*it);
                ve.push_back(v);
                next = false;
            }
            else
            {
                v = hexDigit(*it);
                next = true;
            }
        }

        return ve;
    }

    void Configure::set(const std::wstring &name, const std::wstring &value, bool setChanged)
    {
        confMap[name] = value;
        changed = setChanged;
    }

    void Configure::setBinary(const std::wstring &name, const std::vector<uint8_t> &ve)
    {
        std::wstring str;
        str.reserve(ve.size() * 2);
        for(auto it = ve.begin(); it != ve.end(); ++ it)
        {
            str.push_back(digitHex(*it >> 4));
            str.push_back(digitHex(*it & 0x0F));
        }
        set(name, str);
    }

    void Configure::remove(const std::wstring &name)
    {
        auto it = find(name);
        if(it != confMap.end())
        {
            confMap.erase(it);
            changed = true;
        }
    }
}
