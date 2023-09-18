#include "ReferenceCountingPointer.hpp"

namespace ReferenceCountingPointerUtils {
TAU_UTILS_LIB ReferenceCountDataObject<void*> NullRCDO(nullptr);
TAU_UTILS_LIB SWReferenceCount<void*> NullSWRC(nullptr);
}
