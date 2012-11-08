// $Id$

#include "Common.h"

#include "Primitives.h"

namespace Maragi
{
	namespace UI
	{
		std::map<void *, size_t> Object::objects;

		Object::Object()
			: refCount(1)
		{}

		Object::~Object()
		{}

		void *Object::operator new(size_t size)
		{
			void *ptr = ::operator new(size);
			objects.insert(std::make_pair(ptr, size));
			return ptr;
		}

		void Object::operator delete(void *ptr)
		{
			objects.erase(ptr);
			::operator delete(ptr);
		}

		void Object::addRef()
		{
			++ refCount;
		}

		void Object::release()
		{
			if(-- refCount == 0)
				delete this;
		}
	}
}
