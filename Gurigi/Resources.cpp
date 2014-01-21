#include "Common.h"

#include "Batang/Global.h"

#include "Global.h"
#include "Resources.h"

namespace Gurigi
{
    namespace Resources
    {
        ResourceID::ResourceID(const std::wstring &iname)
        {
            wchar_t *namep = new wchar_t[iname.size() + 1];
            wcscpy_s(namep, iname.size() + 1, iname.c_str());
            name_ = namep;
            allocated_ = true;
        }

        ResourceID::ResourceID(const ResourceID &that)
        {
            if(that.allocated_)
            {
                size_t len = wcslen(that.name_);
                wchar_t *namep = new wchar_t[len + 1];
                wcscpy_s(namep, len + 1, that.name_);
                name_ = namep;
                allocated_ = true;
            }
            else
            {
                name_ = that.name_;
                allocated_ = false;
            }
        }

        ResourceID::ResourceID(ResourceID &&that)
        {
            name_ = that.name_;
            allocated_ = that.allocated_;

            that.name_ = nullptr;
            that.allocated_ = false;
        }

        ResourceID &ResourceID::operator =(const ResourceID &rhs)
        {
            if(&rhs != this)
            {
                if(allocated_)
                    delete [] name_;

                if(rhs.allocated_)
                {
                    size_t len = wcslen(rhs.name_);
                    wchar_t *namep = new wchar_t[len + 1];
                    wcscpy_s(namep, len + 1, rhs.name_);
                    name_ = namep;
                    allocated_ = true;
                }
                else
                {
                    name_ = rhs.name_;
                    allocated_ = false;
                }
            }
            return *this;
        }

        ResourceID &ResourceID::operator =(ResourceID &&rhs)
        {
            if(&rhs != this)
            {
                if(allocated_)
                    delete [] name_;

                name_ = rhs.name_;
                allocated_ = rhs.allocated_;

                rhs.name_ = nullptr;
                rhs.allocated_ = false;
            }
            return *this;
        }

        bool operator ==(const ResourceID &lhs, const ResourceID &rhs)
        {
            if(lhs.allocated_ == rhs.allocated_)
            {
                if(lhs.allocated_)
                    return wcscmp(lhs.name_, rhs.name_) == 0;
                else
                    return lhs.name_ == rhs.name_;
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
            instances.push_back(Batang::Win32Environment::instance().getInstance());
            // TODO: DLL instances

            for(auto it = std::begin(instances); it != std::end(instances); ++ it)
            {
                if(FindResourceW(*it, id, type) != nullptr)
                    return *it;
            }

            return nullptr;
        }

        void ResourcePtrDeleter::operator ()(Resource *ptr) const
        {
            delete ptr;
        }

        Icon::Icon()
            : icon_(nullptr), shared_(false)
        {
        }

        Icon::Icon(HICON iicon, bool ishared)
            : icon_(iicon), shared_(ishared)
        {
        }

        Icon::~Icon()
        {
            if(icon_ != nullptr && !shared_)
                DestroyIcon(icon_);
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

        const ResourcePtr<Cursor> &Cursor::Predefined::arrow()
        {
            static ResourcePtr<Cursor> cursor = Cursor::fromResource(IDC_ARROW);
            return cursor;
        }

        const ResourcePtr<Cursor> &Cursor::Predefined::ibeam()
        {
            static ResourcePtr<Cursor> cursor = Cursor::fromResource(IDC_IBEAM);
            return cursor;
        }

        const ResourcePtr<Cursor> &Cursor::Predefined::wait()
        {
            static ResourcePtr<Cursor> cursor = Cursor::fromResource(IDC_WAIT);
            return cursor;
        }

        const ResourcePtr<Cursor> &Cursor::Predefined::hand()
        {
            static ResourcePtr<Cursor> cursor = Cursor::fromResource(IDC_HAND);
            return cursor;
        }

        const ResourcePtr<Cursor> &Cursor::Predefined::help()
        {
            static ResourcePtr<Cursor> cursor = Cursor::fromResource(IDC_HELP);
            return cursor;
        }

        Cursor::Cursor()
            : cursor_(nullptr)
        {
        }

        Cursor::Cursor(HCURSOR icursor)
            : cursor_(icursor)
        {
        }

        Cursor::~Cursor()
        {
        }

        ResourcePtr<Cursor> Cursor::fromResource(const ResourceID &id)
        {
            HCURSOR cursor = static_cast<HCURSOR>(LoadCursorW(ResourceHelper::findInstanceByResourceID(id, RT_CURSOR), id));
            return ResourcePtr<Cursor>(new Cursor(cursor));
        }

        ResourcePtr<Cursor> Cursor::fromFile(const std::wstring &file)
        {
            HCURSOR cursor = static_cast<HCURSOR>(LoadCursorFromFileW(file.c_str()));
            return ResourcePtr<Cursor>(new Cursor(cursor));
        }
    }
}
