// $Id$

#pragma once

namespace Maragi
{
	// class GlobalInitializerManager;

	/*template<typename T>
	class Singleton // thread-unsafe
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
	};*/

	template<typename T>
	class Singleton // thread-safe
	{
	private:
		class Deleter
		{
		public:
			void operator ()(T *ptr) const
			{
				if(sizeof(T) > 0)
					delete ptr;
			}
		};

	protected:
		~Singleton() {}

	private:
		static std::unique_ptr<T, Deleter> ptr;
		static boost::once_flag onceFlag;
		// static T *ptr;
		// static boost::mutex lock;

	protected:
		// const static std::string uninitializerName;

	public:
		static T &instance()
		{
			/*T *tmp = ptr;
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
			}*/
			boost::call_once(onceFlag, [] { ptr.reset(new T); });
			return *ptr.get();
		}
		static T &inst() { return instance(); }
	};

	/*template<typename T>
	T *SingletonMT<T>::ptr = nullptr;
	template<typename T>
	boost::mutex SingletonMT<T>::lock;
	template<typename T>
	const std::string SingletonMT<T>::uninitializerName = std::string("Singleton Deleter ") + typeid(T).name();*/
	template<typename T>
	std::unique_ptr<T, typename Singleton<T>::Deleter> Singleton<T>::ptr;
	template<typename T>
	boost::once_flag Singleton<T>::onceFlag;

	template<typename T>
	class SingletonLocal // thread-local (using TLS)
	{
	protected:
		~SingletonLocal() {}

	private:
		static void deleter(T *ptr)
		{
			delete ptr;
		}

	public:
		static T &instance()
		{
			static boost::thread_specific_ptr<T> ptr(deleter);
			if(ptr.get() == nullptr)
				ptr.reset(new T);
			return *ptr;
		}
		static T &inst() { return instance(); }
	};
}

// #include "Initializer.h"
