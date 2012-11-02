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
	}
}
