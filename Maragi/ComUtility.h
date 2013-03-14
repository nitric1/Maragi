// $Id: ComUtility.h 79 2012-11-19 20:08:06Z wdlee91 $

#pragma once

#include "Singleton.h"

namespace Maragi
{
	namespace UI
	{
		class ComInitializer : public Singleton<ComInitializer>
		{
		private:
			ComInitializer();
			~ComInitializer();

			friend class Singleton<ComInitializer>;
		};

		template<typename T>
		class ComPtr
		{
			static_assert(std::is_convertible<T *, IUnknown *>::value, "T must be a COM class.");

		private:
			T *ptr;

		public:
			ComPtr()
				: ptr(nullptr)
			{}

			ComPtr(nullptr_t)
				: ptr(nullptr)
			{}

			ComPtr(T *iptr)
				: ptr(iptr)
			{}

			ComPtr(const ComPtr &that)
				: ptr(that.ptr)
			{
				ptr->AddRef();
			}

			ComPtr(ComPtr &&that)
				: ptr(that.ptr)
			{
				that.ptr = nullptr;
			}

			~ComPtr()
			{
				if(ptr)
					ptr->Release();
			}

		public:
			T *get() const
			{
				return ptr;
			}

			T **getPtr()
			{
				return &ptr;
			}

		public:
			operator T *() const
			{
				return ptr;
			}

			T &operator *() const
			{
				assert(ptr != nullptr);
				return *ptr;
			}

			T **operator &()
			{
				return &ptr;
			}

			T *operator ->() const
			{
				return ptr;
			}

			void release()
			{
				if(ptr != nullptr)
				{
					ptr->Release();
					ptr = nullptr;
				}
			}

			ComPtr &operator =(nullptr_t)
			{
				release();
			}

			ComPtr &operator =(const ComPtr &rhs)
			{
				if(ptr != rhs.ptr)
				{
					if(ptr != nullptr)
						ptr->Release();
					ptr = rhs.ptr;
					if(ptr != nullptr)
						ptr->AddRef();
				}
				return *this;
			}

			ComPtr &operator =(ComPtr &&rhs)
			{
				if(ptr != rhs.ptr)
				{
					if(ptr != nullptr)
						ptr->Release();
					ptr = rhs.ptr;
					rhs.ptr = nullptr;
				}
				return *this;
			}
		};

		class ComException : public std::runtime_error
		{
		public:
			ComException()
				: std::runtime_error("ComException")
			{}

			explicit ComException(const std::string &message)
				: std::runtime_error(message)
			{}

			explicit ComException(const char *message)
				: std::runtime_error(message)
			{}
		};
	}
}
