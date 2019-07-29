#pragma once

#include "../Batang/Singleton.h"

#include "Window.h"

namespace Gurigi
{
    class ShortcutKey final : public Batang::Singleton<ShortcutKey>
    {
    public:
        enum class Modifier : uint8_t
        {
            None = 0,
            Ctrl = 1,
            Alt = 2,
            Shift = 4,
            Ctrl_Alt = Ctrl | Alt,
            Ctrl_Shift = Ctrl | Shift,
            Alt_Shift = Alt | Shift,
            Ctrl_Alt_Shift = Ctrl | Alt | Shift,
        };

        struct Key
        {
            Modifier modifier;
            uint32_t key;

            bool operator <(const Key &rhs) const
            {
                return modifier < rhs.modifier || (modifier == rhs.modifier && key < rhs.key);
            }

            explicit operator bool() const
            {
                return key != 0;
            }
        };

    private:
        struct ConfigureData
        {
            struct ConfigureField
            {
            };

            uint32_t len;
            ConfigureField f[1];
        };

        struct Command
        {
            bool canExtended;
            bool canRepeated;
            uint32_t id;
        };

    private:
        std::map<std::pair<Key, const Shell *>, Command> keyMap_;

    private:
        ShortcutKey();
        ~ShortcutKey();

    public:
        static Modifier getModifier(bool, bool, bool);
        static bool verifyKey(Modifier, uint32_t);
        static Key makeKey(Modifier, uint32_t);

    public:
        void load(const std::vector<uint8_t> &);
        bool addShortcut(const Shell *, uint32_t, Key, bool = true, bool = true);
        bool modifyShortcut(const Shell *, uint32_t, Key, bool, bool);
        bool removeShortcut(const Shell *, uint32_t);
        uint32_t processShortcut(const Shell *, Key, bool, bool) const;
        bool processKey(const Shell *, bool, bool, bool, uint32_t, bool, uint32_t) const;

        friend class Batang::Singleton<ShortcutKey>;
    };
}
