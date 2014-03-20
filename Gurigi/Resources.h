#pragma once

#include "Objects.h"
#include "Utility.h"

namespace Gurigi
{
    namespace Resources
    {
        class ResourceId
        {
        private:
            union
            {
                uint16_t id_;
                const wchar_t *name_;
            };
            bool allocated_;

        public:
            ResourceId(uint16_t id)
                : id_(id), allocated_(false)
            {}

            ResourceId(const wchar_t *name)
                : name_(name), allocated_(false)
            {}

            ResourceId(const std::wstring &);
            ResourceId(const ResourceId &);
            ResourceId(ResourceId &&);

            ~ResourceId()
            {
                if(allocated_)
                    delete [] name_;
            }

        public:
            ResourceId &operator =(const ResourceId &);
            ResourceId &operator =(ResourceId &&);

            operator uint16_t() const
            {
                return id_;
            }

            operator const wchar_t *() const
            {
                return name_;
            }

            friend bool operator ==(const ResourceId &, const ResourceId &);
            friend bool operator !=(const ResourceId &, const ResourceId &);
        };

        class ResourceHelper
        {
        private:
            ~ResourceHelper() = delete;

        public:
            static HINSTANCE findInstanceByResourceId(const ResourceId &, const ResourceId &);
        };

        class Resource;

        struct ResourcePtrDeleter
        {
            void operator ()(Resource *) const;
        };

        template<typename = Resource>
        class ResourcePtr;

        /*template<typename T>
        class ResourcePtr : public std::shared_ptr<T>
        {
            static_assert(std::is_convertible<T *, Resource *>::value, "T must be a derived class from Maragi::UI::Resources::Resource.");

        public:
            ResourcePtr()
                : std::shared_ptr<T>(nullptr, ResourcePtrDeleter())
            {}

            explicit ResourcePtr(T *ptr)
                : std::shared_ptr<T>(ptr, ResourcePtrDeleter())
            {}

            ResourcePtr(nullptr_t)
                : std::shared_ptr<T>(nullptr, ResourcePtrDeleter())
            {}

            template<typename Other>
            ResourcePtr(const ResourcePtr<Other> &that)
                : std::shared_ptr<T>(that)
            {}

            template<typename Other>
            ResourcePtr(ResourcePtr<Other> &&that)
                : std::shared_ptr<T>(std::forward<ResourcePtr<Other>>(that))
            {}
        };*/

        template<typename T>
        class ResourcePtr : public SharedPtr<Resource, ResourcePtrDeleter, T>
        {
        public:
            ResourcePtr()
                : SharedPtr<Resource, ResourcePtrDeleter, T>()
            {}

            ResourcePtr(Resource *iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(iptr)
            {}

            template<typename Other>
            ResourcePtr(const ResourcePtr<Other> &that)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(that)
            {}

            template<typename Other>
            ResourcePtr(ResourcePtr<Other> &&that)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(std::move(that))
            {}

            ResourcePtr(const std::shared_ptr<Resource> &iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(iptr)
            {}

            ResourcePtr(std::shared_ptr<Resource> &&iptr)
                : SharedPtr<Resource, ResourcePtrDeleter, T>(std::move(iptr))
            {}

            friend class Resource;
            template<typename Other>
            friend class ResourcePtr;
        };

        class Resource
        {
        protected:
            virtual ~Resource() = 0
            {}

        protected:
            Resource &operator =(const Resource &) = delete;

            friend struct ResourcePtrDeleter;
        };

        class Icon : public Resource
        {
        private:
            HICON icon_;
            bool shared_;

        protected:
            Icon();
            Icon(HICON, bool);
            virtual ~Icon();

        public:
            static ResourcePtr<Icon> fromResource(const ResourceId &);
            static ResourcePtr<Icon> fromResource(const ResourceId &, const Objects::SizeI &);
            static ResourcePtr<Icon> fromSharedResource(const ResourceId &);
            static ResourcePtr<Icon> fromSharedResource(const ResourceId &, const Objects::SizeI &);
            static ResourcePtr<Icon> fromFile(const std::wstring &);
            static ResourcePtr<Icon> fromFile(const std::wstring &, const Objects::SizeI &);

        public:
            HICON getIcon() const
            {
                return icon_;
            }

            operator HICON() const
            {
                return icon_;
            }
        };

        class Cursor : public Resource
        {
        private:
            HCURSOR cursor_;

        public:
            struct Predefined
            {
                static const ResourcePtr<Cursor> &arrow();
                static const ResourcePtr<Cursor> &ibeam();
                static const ResourcePtr<Cursor> &wait();
                static const ResourcePtr<Cursor> &hand();
                static const ResourcePtr<Cursor> &help();
            };

        protected:
            Cursor();
            Cursor(HCURSOR);
            virtual ~Cursor();

        public:
            static ResourcePtr<Cursor> fromResource(const ResourceId &);
            static ResourcePtr<Cursor> fromFile(const std::wstring &);

        public:
            HCURSOR getCursor() const
            {
                return cursor_;
            }

            operator HCURSOR() const
            {
                return cursor_;
            }
        };
    }
}
