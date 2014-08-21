#include "Common.h"

#include "Batang/Utility.h"

#include "Utility.h"
#include "Configure.h"

namespace Maragi
{
    const std::wstring Configure::ConfFileName = L"Maragi.conf";

    namespace
    {
        struct FileCloser
        {
            void operator ()(HANDLE file) const
            {
                if(file != INVALID_HANDLE_VALUE)
                    CloseHandle(file);
            }
        };
    }

    Configure::Configure()
        : changed_(false)
    {
        std::unique_ptr<void, FileCloser> file(
            CreateFileW(getConfigurePath().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        if(file.get() == INVALID_HANDLE_VALUE)
            return;

        size_t fileSize = GetFileSize(file.get(), nullptr);
        if(fileSize == 0)
            return;

        std::vector<wchar_t> buffer(fileSize / sizeof(wchar_t));
        ulong32_t read = 0;
        if(!ReadFile(file.get(), &*buffer.begin(), static_cast<ulong32_t>(fileSize), &read, nullptr))
            return;
        buffer.resize(read / sizeof(wchar_t));
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

            name = Batang::trim(std::move(name));
            value = Batang::trim(std::move(value));

            set(name, value, false);
        }
    }

    Configure::~Configure()
    {
        if(!changed_)
            return;

        std::wstring str(L"\xFEFF");
        for(auto it = confMap_.begin(); it != confMap_.end(); ++ it)
        {
            str += it->first;
            str += L"=";
            str += it->second;
            str += L"\n";
        }

        std::unique_ptr<void, FileCloser> file(
            CreateFileW(getConfigurePath().c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
        if(file.get() == INVALID_HANDLE_VALUE)
            return;

        ulong32_t written = 0;
        WriteFile(file.get(), str.c_str(), static_cast<ulong32_t>(str.size() * sizeof(wchar_t)), &written, nullptr);
    }

    std::wstring Configure::getConfigurePath()
    {
        return getDataDirectoryPath() + ConfFileName;
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
        return confMap_.find(name);
    }

    std::map<std::wstring, std::wstring>::const_iterator Configure::find(const std::wstring &name) const
    {
        return confMap_.find(name);
    }

    bool Configure::exists(const std::wstring &name) const
    {
        return find(name) != confMap_.end();
    }

    std::wstring Configure::get(const std::wstring &name, const std::wstring &def) const
    {
        auto it = find(name);
        if(it == confMap_.end())
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

    size_t Configure::getBinary(const std::wstring &name, void *buf, size_t size) const
    {
        std::wstring str = get(name);
        uint8_t *p = static_cast<uint8_t *>(buf);
        uint8_t v = 0;
        bool next = false;
        size_t retSize = std::min(str.size(), size * 2);

        for(size_t i = 0; i < retSize; ++ i)
        {
            if(next)
            {
                v <<= 4;
                v |= hexDigit(str[i]);
                *p ++ = v;
                next = false;
            }
            else
            {
                v = hexDigit(str[i]);
                next = true;
            }
        }

        return retSize / 2;
    }

    void Configure::set(const std::wstring &name, const std::wstring &value, bool setChanged)
    {
        confMap_[name] = value;
        changed_ = setChanged;
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

    void Configure::setBinary(const std::wstring &name, const void *buf, size_t size)
    {
        std::wstring str;
        str.reserve(size * 2);
        const uint8_t *p = static_cast<const uint8_t *>(buf);
        for(; size --; ++ p)
        {
            str.push_back(digitHex(*p >> 4));
            str.push_back(digitHex(*p & 0x0F));
        }
        set(name, str);
    }

    void Configure::remove(const std::wstring &name)
    {
        auto it = find(name);
        if(it != confMap_.end())
        {
            confMap_.erase(it);
            changed_ = true;
        }
    }
}
