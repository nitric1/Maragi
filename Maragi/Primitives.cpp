// $Id$

#include "Common.h"

#include "Primitives.h"

namespace Maragi
{
	namespace UI
	{
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
	}
}
