#include "ReferenceCountingPointer.hpp"

namespace ReferenceCountingPointerUtils {
ReferenceCountDataObject<void*> NullRCDO(nullptr);
SWReferenceCount<void*> NullSWRC(nullptr);
}
