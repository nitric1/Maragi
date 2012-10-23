// $Id$

#pragma once

#include "../Singleton.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class ShortcutKey : public Singleton<ShortcutKey>
		{
		public:
			enum Modifier
			{
				None = 0,
				Ctrl = 1,
				Alt = 2,
				Shift = 4
				Ctrl_Alt = 3,
				Ctrl_Shift = 5,
				Alt_Shift = 6,
				Ctrl_Alt_Shiftb = 7
			};

			struct Key
			{
				Modifier modifier;
				uint32_t key;

				bool operator <(const Key &rhs) const
				{
					return modifier < rhs.modifier || (modifier == rhs.modifier && key < rhs.key);
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
			std::map<std::pair<Key, const Window *>, Command> keyMap;

		private:
			ShortcutKey();
			~ShortcutKey();

		public:
			static Modifier getModifier(bool, bool, bool);
			static bool verifyKey(Modifier, uint32_t);
			static Key makeKey(Modifier, uint32_t);

		public:
			bool addShortcut(const Window *, uint32_t, Key, bool = true, bool = true);
			bool modifyShortcut(const Window *, uint32_t, Key, bool, bool);
			bool removeShortcut(const Window *, uint32_t);
			uint32_t processShortcut(const Window *, Key, bool, bool) const;
			bool processKey(const Window *, bool, bool, bool, uint32_t, bool, uint32_t) const;

			friend class Singleton<ShortcutKey>;
		};
	}
}
