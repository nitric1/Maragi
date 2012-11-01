// $Id$

#pragma once

namespace Maragi
{
	namespace UI
	{
		struct ControlID
		{
			static const ControlID undefined;

			uintptr_t id;

			ControlID()
				: id(0)
			{
			}

			explicit ControlID(uintptr_t iid)
				: id(iid)
			{
			}

			ControlID(const ControlID &that)
				: id(that.id)
			{
			}

			operator bool() const
			{
				return id != 0;
			}

			bool operator !() const
			{
				return id == 0;
			}

			bool operator <(const ControlID &rhs) const
			{
				return id < rhs.id;
			}

			bool operator >(const ControlID &rhs) const
			{
				return id > rhs.id;
			}

			bool operator <=(const ControlID &rhs) const
			{
				return id <= rhs.id;
			}

			bool operator >=(const ControlID &rhs) const
			{
				return id >= rhs.id;
			}

			ControlID &operator =(const ControlID &rhs)
			{
				id = rhs.id;
				return *this;
			}

			ControlID &operator =(uintptr_t iid)
			{
				id = iid;
				return *this;
			}
		};

		class ResourceID
		{
			union
			{
				uint16_t id;
				const wchar_t *name;
			};
			bool allocated;

		public:
			ResourceID(uint16_t iid)
				: id(iid), allocated(false)
			{
			}

			ResourceID(const wchar_t *iname)
				: name(iname), allocated(false)
			{
			}

			ResourceID(const std::wstring &iname)
			{
				wchar_t *namep = new wchar_t[iname.size() + 1];
				wcscpy_s(namep, iname.size() + 1, iname.c_str());
				name = namep;
				allocated = true;
			}

			ResourceID(const ResourceID &that)
			{
				if(that.allocated)
				{
					size_t len = wcslen(that.name);
					wchar_t *namep = new wchar_t[len + 1];
					wcscpy_s(namep, len + 1, that.name);
					name = namep;
					allocated = true;
				}
				else
				{
					name = that.name;
					allocated = false;
				}
			}

			~ResourceID()
			{
				if(allocated)
					delete [] name;
			}

		public:
			ResourceID &operator =(const ResourceID &rhs)
			{
				if(&rhs != this)
				{
					if(allocated)
						delete [] name;

					if(rhs.allocated)
					{
						size_t len = wcslen(rhs.name);
						wchar_t *namep = new wchar_t[len + 1];
						wcscpy_s(namep, len + 1, rhs.name);
						name = namep;
						allocated = true;
					}
					else
					{
						name = rhs.name;
						allocated = false;
					}
				}
				return *this;
			}

			ResourceID &operator =(ResourceID &&rhs)
			{
				if(&rhs != this)
				{
					if(allocated)
						delete [] name;

					name = rhs.name;
					allocated = rhs.allocated;

					rhs.name = nullptr;
					rhs.allocated = false;
				}
				return *this;
			}

			operator uint16_t() const
			{
				return id;
			}

			operator const wchar_t *() const
			{
				return name;
			}

			friend bool operator ==(const ResourceID &, const ResourceID &);
			friend bool operator !=(const ResourceID &, const ResourceID &);
		};
	}
}
