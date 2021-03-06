﻿#pragma once

#include "Window.h"

namespace Gurigi
{
    class Dialog : public Shell
    {
    private:
        enum { WM_APP_ENDDIALOG = WM_APP + 100 };

    private:
        std::vector<uint8_t> dlgData_;
        bool isDialogEnd_;
        intptr_t endDialogResult_;

    public:
        Dialog();
        explicit Dialog(const ShellWeakPtr<> &);

    protected:
        virtual ~Dialog() = 0;

    protected:
        virtual const wchar_t *dialogName() = 0;
        virtual const void *dialogTemplateWithSystemFont();
        virtual HWND itemHandle(int32_t);

        intptr_t showModal(const ERDelegateWrapper<intptr_t (HWND, unsigned, uintptr_t, longptr_t)> &, int = SW_SHOW);
        bool endDialog(intptr_t);
    };
}
