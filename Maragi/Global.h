// $Id$

#pragma once

#include "Singleton.h"

namespace Maragi
{
	class Environment : public Singleton<Environment>
	{
	private:
		HINSTANCE hinst;

	private:
		Environment();

	public:
		HINSTANCE getInstance() const
		{
			return hinst;
		}

		// TODO: DLL instances

		friend class Singleton<Environment>;
	};

	class Initializer
	{
	public:
		virtual std::string getName() const = 0;
		virtual void init() const = 0;
		virtual void uninit() const = 0;
	};

	class InstantInitializer : public Initializer
	{
	private:
		std::string name;
		std::function<void ()> initFn, uninitFn;

	public:
		InstantInitializer(const std::string &iname, const std::function<void ()> &iinitFn, const std::function<void ()> &iuninitFn)
			: name(iname), initFn(iinitFn), uninitFn(iuninitFn)
		{}

		virtual std::string getName() const
		{
			return name;
		}

		virtual void init() const
		{
			initFn();
		}

		virtual void uninit() const
		{
			uninitFn();
		}
	};

	class GlobalInitializerManager : public Singleton<GlobalInitializerManager>
	{
	private:
		std::map<std::string, std::shared_ptr<Initializer>> inits;
		boost::mutex lock;

	private:
		GlobalInitializerManager()
		{}

	private:
		GlobalInitializerManager(const GlobalInitializerManager &); // = delete;

	public:
		void add(const std::shared_ptr<Initializer> &);
		void add(const std::string &name, const std::function<void ()> &initFn, const std::function<void ()> &uninitFn) // using InstantInitializer
		{
			add(std::shared_ptr<Initializer>(new InstantInitializer(name, initFn, uninitFn)));
		}

	private:
		void uninit();

		friend class Singleton<GlobalInitializerManager>;
		friend class GlobalUninitializer;
	};

	class GlobalUninitializer
	{
	public:
		~GlobalUninitializer()
		{
			GlobalInitializerManager::instance().uninit();
		}
	};

	extern GlobalUninitializer globalUninitializer;

	/* class ThreadInitializer
	{
	}; */
}
