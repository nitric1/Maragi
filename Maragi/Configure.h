#pragma once

#include "Batang/Singleton.h"

namespace Maragi
{
    class Configure final : public Batang::Singleton<Configure>
    {
    private:
        static const std::wstring ConfFileName;

        std::map<std::wstring, std::wstring> confMap_;
        bool changed_;

    private:
        Configure();
        ~Configure();

    private:
        static std::wstring getConfigurePath();

    private:
        std::map<std::wstring, std::wstring>::iterator find(const std::wstring &);
        std::map<std::wstring, std::wstring>::const_iterator find(const std::wstring &) const;

    public:
        bool exists(const std::wstring &) const;
        std::wstring get(const std::wstring &, const std::wstring & = std::wstring()) const;
        std::vector<uint8_t> getBinary(const std::wstring &) const;
        size_t getBinary(const std::wstring &, void *, size_t) const;
        void set(const std::wstring &, const std::wstring &, bool = true);
        void setBinary(const std::wstring &, const std::vector<uint8_t> &);
        void setBinary(const std::wstring &, const void *, size_t);
        void remove(const std::wstring &);

        friend class Batang::Singleton<Configure>;
    };
}
