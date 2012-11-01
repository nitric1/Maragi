// $Id$

#include "Common.h"

#include "Global.h"
#include "Objects.h"

namespace Maragi
{
	namespace UI
	{
		namespace Objects
		{
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
				// TODO: width/height
				HICON icon = static_cast<HICON>(LoadImageW(Environment::instance().getInstance(), id, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
				return ResourcePtr<Icon>(new Icon(icon, false));
			}
		}
	}
}
