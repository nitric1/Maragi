// $Id$

#pragma once

#include "Objects.h"

namespace Maragi
{
	namespace UI
	{
		namespace Resources
		{
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
				{}

				ResourceID(const wchar_t *iname)
					: name(iname), allocated(false)
				{}

				ResourceID(const std::wstring &);
				ResourceID(const ResourceID &);

				~ResourceID()
				{
					if(allocated)
						delete [] name;
				}

			public:
				ResourceID &operator =(const ResourceID &);
				ResourceID &operator =(ResourceID &&);

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

			class ResourceHelper
			{
			private:
				~ResourceHelper(); // = delete;

			public:
				static HINSTANCE findInstanceByResourceID(const ResourceID &, const ResourceID &);
			};

			class Resource;

			template<typename = Resource>
			class ResourcePtr;

			template<typename T>
			class ResourcePtr : public std::shared_ptr<T>
			{
			private:
				struct Deleter
				{
					void operator ()(T *ptr) const
					{
						ptr->release();
					}
				};

			public:
				ResourcePtr()
					: std::shared_ptr<T>()
				{}

				explicit ResourcePtr(T *ptr)
					: std::shared_ptr<T>(ptr)
				{}

				ResourcePtr(nullptr_t)
					: std::shared_ptr<T>(nullptr)
				{}

				template<typename Other>
				ResourcePtr(const ResourcePtr<Other> &that)
					: std::shared_ptr<T>(that)
				{}

				template<typename Other>
				ResourcePtr(ResourcePtr<Other> &&that)
					: std::shared_ptr<T>(that)
				{}
			};

			class Resource
			{
			private:
				virtual void release()
				{
					delete this;
				}

				friend class ResourcePtr<Resource>;
			};

			class Icon : public Resource
			{
			private:
				HICON icon;
				bool shared;

			private:
				Icon();
				Icon(HICON, bool);

			public:
				virtual ~Icon();

			public:
				static ResourcePtr<Icon> fromResource(const ResourceID &);
				static ResourcePtr<Icon> fromResource(const ResourceID &, const Objects::SizeI &);
				static ResourcePtr<Icon> fromSharedResource(const ResourceID &);
				static ResourcePtr<Icon> fromSharedResource(const ResourceID &, const Objects::SizeI &);
				static ResourcePtr<Icon> fromFile(const std::wstring &);
				static ResourcePtr<Icon> fromFile(const std::wstring &, const Objects::SizeI &);

			public:
				HICON getIcon() const
				{
					return icon;
				}

				operator HICON() const
				{
					return icon;
				}
			};
		}
	}
}
