// $Id$

#pragma once

#include "Singleton.h"

namespace Maragi
{
	class Environment : public Singleton<Environment>
	{
	private:
		HINSTANCE inst_; // Main instance
		std::set<HINSTANCE> dllInstances; // DLLs' instance

	private:
		Environment();

	public:
		void addDllInstance(HINSTANCE dllInst)
		{
			dllInstances.insert(dllInst);
		}

		void removeDllInstance(HINSTANCE dllInst)
		{
			dllInstances.erase(dllInst);
		}

		HINSTANCE getInstance() const
		{
			return inst_;
		}

		const std::set<HINSTANCE> &getDllInstances() const
		{
			return dllInstances;
		}

		friend class Singleton<Environment>;
	};

	class Initializer
	{
	public:
		virtual std::string getName() const
		{
			return typeid(*this).name();
		}

		virtual void init() = 0;
		virtual void uninit() = 0;
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

		virtual void init()
		{
			initFn();
		}

		virtual void uninit()
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
		~GlobalInitializerManager();

	private:
		GlobalInitializerManager(const GlobalInitializerManager &); // = delete;

	public:
		void add(const std::shared_ptr<Initializer> &);
		void add(const std::string &name, const std::function<void ()> &initFn, const std::function<void ()> &uninitFn) // using InstantInitializer
		{
			add(std::shared_ptr<Initializer>(new InstantInitializer(name, initFn, uninitFn)));
		}

		friend class Singleton<GlobalInitializerManager>;
	};
}
