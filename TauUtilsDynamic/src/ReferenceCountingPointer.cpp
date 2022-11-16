#include "ReferenceCountingPointer.hpp"

namespace ReferenceCountingPointerUtils {
TAU_LIB ReferenceCountDataObject<void*> NullRCDO(nullptr);
TAU_LIB SWReferenceCount<void*> NullSWRC(nullptr);
}
