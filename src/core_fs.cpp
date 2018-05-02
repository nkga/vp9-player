#include "core_fs.h"
#include "core_heap.h"
#include <stdlib.h>

namespace core {
namespace fs {

void* read(WCHAR const* path, u32* bytes) {
	*bytes = 0;

	HANDLE fp = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	LARGE_INTEGER result;
	if (GetFileSizeEx(fp, &result) == FALSE || result.QuadPart > 0xFFFFFFFF) {
		CloseHandle(fp);
		return nullptr;
	}

	DWORD total = (DWORD)result.QuadPart;
	void* buffer = heap::raw_calloc(total);
	
	DWORD read;
	if (ReadFile(fp, buffer, total, &read, nullptr) == FALSE || read != total) {
		CloseHandle(fp);
		return nullptr;
	}

	*bytes = total;
	CloseHandle(fp);

	return buffer;;
}

}
}
