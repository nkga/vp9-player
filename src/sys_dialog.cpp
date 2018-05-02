#include "sys_dialog.h"
#include <windows.h>
#include <shobjidl.h>
#include <CommCtrl.h>

namespace sys {
namespace dialog {

void startup() {
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

WCHAR* open() {
	static WCHAR result[1024];

	IFileOpenDialog* pfd = nullptr;
	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
		return result;
	}

	static const COMDLG_FILTERSPEC spec[] = {
		{ L"IVF Files", L"*.ivf" },
		{ L"All Files", L"*.*" },
	};

	pfd->SetFileTypes(ARRAYSIZE(spec), spec);
	pfd->SetFileTypeIndex(0);

	if (SUCCEEDED(pfd->Show(NULL))) {
		IShellItem *item;
		if (SUCCEEDED(pfd->GetResult(&item))) {
			PWSTR path;
			if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
				wcsncpy_s(result, path, _TRUNCATE);
				CoTaskMemFree(path);
			}

			item->Release();
		}

		pfd->Release();
	}

	return result;
}

}
}
