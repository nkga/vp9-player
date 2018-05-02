#pragma once
#include "core.h"

namespace sys {
namespace com {

template <typename tval>
static inline void release(tval** p) {
	if (*p) {
		(*p)->Release();
		*p = nullptr;
	}
}

}
}
