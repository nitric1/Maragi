#include "Common.h"

#include "ShortcutKey.h"

using namespace std;

namespace Gurigi
{
    ShortcutKey::ShortcutKey()
    {
    }

    ShortcutKey::~ShortcutKey()
    {
    }

    ShortcutKey::Modifier ShortcutKey::getModifier(bool ctrl, bool alt, bool shift)
    {
        if(ctrl && alt && shift)
            return Modifier::Ctrl_Alt_Shift;
        else if(ctrl && alt)
            return Modifier::Ctrl_Alt;
        else if(ctrl && shift)
            return Modifier::Ctrl_Shift;
        else if(alt && shift)
            return Modifier::Alt_Shift;
        else if(ctrl)
            return Modifier::Ctrl;
        else if(alt)
            return Modifier::Alt;
        else if(shift)
            return Modifier::Shift;
        return Modifier::None;
    }

    namespace
    {
        bool isNormalKey(uint32_t key)
        {
            if(key == VK_SPACE)
                return true;
            else if(0x30 <= key && key <= 0x39) // 0 - 9
                return true;
            else if(0x41 <= key && key <= 0x5A) // A - Z
                return true;
            /*
            // Well, numpad keys should not be treated as normal key...
            else if(VK_NUMPAD0 <= key && key <= VK_NUMPAD9)
                return true;
            else if(key == VK_MULTIPLY || key == VK_ADD || key == VK_SUBTRACT || key == VK_DIVIDE || key == VK_DECIMAL) // Numpad * + - / .
                return true;
            */
            else if(key == VK_OEM_1 || key == VK_OEM_PLUS || key == VK_OEM_COMMA || key == VK_OEM_MINUS || key == VK_OEM_PERIOD
                || key == VK_OEM_2 || key == VK_OEM_3 || key == VK_OEM_4 || key == VK_OEM_5 || key == VK_OEM_6
                || key == VK_OEM_7 || key == VK_OEM_8 || key == VK_OEM_102)
                return true;
            return false;
        }
    }

    bool ShortcutKey::verifyKey(Modifier modifier, uint32_t key)
    {
        if(modifier == Modifier::None && isNormalKey(key))
            return false;
        else if(modifier == Modifier::Shift && isNormalKey(key))
            return false;
        return true;
    }

    ShortcutKey::Key ShortcutKey::makeKey(Modifier modifier, uint32_t key)
    {
        if(!verifyKey(modifier, key))
            return Key();
        Key skey = {modifier, key};
        return skey;
    }

    void ShortcutKey::load(const std::vector<uint8_t> &dataVec)
    {
        if(dataVec.empty())
            return;

        const ConfigureData *data = reinterpret_cast<const ConfigureData *>(&*dataVec.begin());
    }

    // Extended key: other side's key (e.g. numpad keys, right-hand control keys, ...)
    bool ShortcutKey::addShortcut(const Shell *window, uint32_t id, Key key, bool canExtended, bool canRepeated)
    {
        Command cmd;
        cmd.canExtended = canExtended;
        cmd.canRepeated = canRepeated;
        cmd.id = id;

        return keyMap_.insert(make_pair(make_pair(key, window), cmd)).second;
    }

    bool ShortcutKey::modifyShortcut(const Shell *window, uint32_t id, Key key, bool canExtended, bool canRepeated)
    {
        if(!removeShortcut(window, id))
            return false;
        return addShortcut(window, id, key, canExtended, canRepeated);
    }

    bool ShortcutKey::removeShortcut(const Shell *window, uint32_t id)
    {
        for(auto it = keyMap_.begin(); it != keyMap_.end(); ++ it)
        {
            if(it->first.second == window && it->second.id == id)
            {
                keyMap_.erase(it);
                return true;
            }
        }

        return false;
    }

    uint32_t ShortcutKey::processShortcut(const Shell *window, Key key, bool extended, bool repeated) const
    {
        auto it = keyMap_.find(make_pair(key, window));
        if(it == keyMap_.end())
            return 0;

        if((extended && !it->second.canExtended) || (repeated && !it->second.canRepeated))
            return 0;

        return it->second.id;
    }

    bool ShortcutKey::processKey(const Shell *window, bool ctrl, bool alt, bool shift, uint32_t keycode, bool extended, uint32_t repeat) const
    {
        Key key;
        key.modifier = getModifier(ctrl, alt, shift);
        key.key = keycode;

        uint32_t id = processShortcut(window, key, extended, repeat > 1);
        if(id == 0)
            return false;

        return PostMessageW(window->hwnd(), WM_COMMAND, id, 0) != FALSE;
    }
}
