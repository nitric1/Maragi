// $Id$

#include "Common.h"

#include "Global.h"
#include "Resources.h"

namespace Maragi
{
	namespace UI
	{
		namespace Resources
		{
			ResourceID::ResourceID(const std::wstring &iname)
			{
				wchar_t *namep = new wchar_t[iname.size() + 1];
				wcscpy_s(namep, iname.size() + 1, iname.c_str());
				name = namep;
				allocated = true;
			}

			ResourceID::ResourceID(const ResourceID &that)
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

			ResourceID &ResourceID::operator =(const ResourceID &rhs)
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

			ResourceID &ResourceID::operator =(ResourceID &&rhs)
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

			bool operator ==(const ResourceID &lhs, const ResourceID &rhs)
			{
				if(lhs.allocated == rhs.allocated)
				{
					if(lhs.allocated)
						return wcscmp(lhs.name, rhs.name) == 0;
					else
						return lhs.name == rhs.name;
				}

				return false;
			}

			bool operator !=(const ResourceID &lhs, const ResourceID &rhs)
			{
				return !(lhs == rhs);
			}

			HINSTANCE ResourceHelper::findInstanceByResourceID(const ResourceID &id, const ResourceID &type)
			{
				std::vector<HINSTANCE> instances;
				instances.push_back(Environment::instance().getInstance());
				// TODO: DLL instances

				for(auto it = std::begin(instances); it != std::end(instances); ++ it)
				{
					if(FindResource(*it, id, type) != nullptr)
						return *it;
				}

				return nullptr;
			}

			Icon::Icon()
				: icon(nullptr), shared(false)
			{
			}

			Icon::Icon(HICON iicon, bool ishared)
				: icon(iicon), shared(ishared)
			{
			}

			Icon::~Icon()
			{
				if(icon != nullptr && !shared)
					DestroyIcon(icon);
			}

			ResourcePtr<Icon> Icon::fromResource(const ResourceID &id)
			{
				HICON icon = static_cast<HICON>(LoadImageW(ResourceHelper::findInstanceByResourceID(id, RT_ICON), id, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR));
				return ResourcePtr<Icon>(new Icon(icon, false));
			}

			ResourcePtr<Icon> Icon::fromResource(const ResourceID &id, const Objects::SizeI &desiredSize)
			{
				HICON icon = static_cast<HICON>(LoadImageW(ResourceHelper::findInstanceByResourceID(id, RT_ICON), id, IMAGE_ICON, desiredSize.width, desiredSize.height, LR_DEFAULTCOLOR));
				return ResourcePtr<Icon>(new Icon(icon, false));
			}

			ResourcePtr<Icon> Icon::fromSharedResource(const ResourceID &id)
			{
				HICON icon = static_cast<HICON>(LoadImageW(ResourceHelper::findInstanceByResourceID(id, RT_ICON), id, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED));
				return ResourcePtr<Icon>(new Icon(icon, true));
			}

			ResourcePtr<Icon> Icon::fromSharedResource(const ResourceID &id, const Objects::SizeI &desiredSize)
			{
				HICON icon = static_cast<HICON>(LoadImageW(ResourceHelper::findInstanceByResourceID(id, RT_ICON), id, IMAGE_ICON, desiredSize.width, desiredSize.height, LR_DEFAULTCOLOR | LR_SHARED));
				return ResourcePtr<Icon>(new Icon(icon, true));
			}

			ResourcePtr<Icon> Icon::fromFile(const std::wstring &file)
			{
				HICON icon = static_cast<HICON>(LoadImageW(nullptr, file.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_LOADFROMFILE));
				return ResourcePtr<Icon>(new Icon(icon, false));
			}

			ResourcePtr<Icon> Icon::fromFile(const std::wstring &file, const Objects::SizeI &desiredSize)
			{
				HICON icon = static_cast<HICON>(LoadImageW(nullptr, file.c_str(), IMAGE_ICON, desiredSize.width, desiredSize.height, LR_DEFAULTCOLOR | LR_LOADFROMFILE));
				return ResourcePtr<Icon>(new Icon(icon, false));
			}
		}
	}
}
