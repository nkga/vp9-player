#pragma once
#include "core.h"
#include <Windows.h>

namespace core {
namespace fs {

void* read(WCHAR const* path, u32* bytes);

}
}
