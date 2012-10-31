// $Id$

#pragma once

namespace Maragi
{
	class GlobalInitializerManager;

	template<typename T>
	class Singleton
	{
	protected:
		~Singleton() {}

	public:
		static T &instance()
		{
			static T theInstance;
			return theInstance;
		}
		static T &inst() { return instance(); }
	};

	template<typename T>
	class SingletonMT // thread-safe
	{
	protected:
		~SingletonMT() {}

	private:
		static T *ptr;
		static boost::mutex lock;

	protected:
		const static std::string uninitializerName;

	public:
		static T &instance()
		{
			// TODO: replace by std::atomic w/ VS 2012.
			T *tmp = ptr;
			_ReadWriteBarrier();
			if(tmp == nullptr)
			{
				boost::mutex::scoped_lock sl(lock);
				tmp = ptr;
				if(tmp == nullptr)
				{
					tmp = new T();
					_ReadWriteBarrier();
					ptr = tmp;
					GlobalInitializerManager::instance().add(uninitializerName, nullptr, []() { delete ptr; });
				}
			}
			return *tmp;
		}
		static T &inst() { return instance(); }
	};

	template<typename T>
	T *SingletonMT<T>::ptr = nullptr;
	template<typename T>
	boost::mutex SingletonMT<T>::lock;
	template<typename T>
	const std::string SingletonMT<T>::uninitializerName = std::string("Singleton Deleter ") + typeid(T).name();

	/*template<typename T>
	class SingletonLocal // thread-local (using TLS)
	{
	};*/
}

#include "Initializer.h"
