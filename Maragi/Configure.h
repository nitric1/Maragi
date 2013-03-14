// $Id: Configure.h 42 2012-08-04 18:23:44Z wdlee91 $

#pragma once

#include "Singleton.h"

namespace Maragi
{
	class Configure : public Singleton<Configure>
	{
	private:
		std::map<std::wstring, std::wstring> confMap;
		static const std::wstring confFileName;
		bool changed;

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
		void set(const std::wstring &, const std::wstring &, bool = true);
		void setBinary(const std::wstring &, const std::vector<uint8_t> &);
		void remove(const std::wstring &);

		friend class Singleton<Configure>;
	};
}
