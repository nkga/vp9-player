#include "sys_window.h"
#include "core_util.h"
#include <windowsx.h>

namespace sys {

window_t::window_t() {
	WNDCLASSW wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndproc;
	wc.hInstance = GetModuleHandleW(0);
	wc.hCursor = LoadCursorW(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = L"wc_client";
	RegisterClassW(&wc);

	const DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	RECT wr = {};
	wr.right = 1280;
	wr.bottom = 720;

	AdjustWindowRect(&wr, style, FALSE);

	POINT pt = {};
	HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi = {};
	mi.cbSize = sizeof(mi);
	GetMonitorInfoW(monitor, &mi);

	RECT work = mi.rcWork;

	i32 w = wr.right - wr.left;
	i32 h = wr.bottom - wr.top;
	i32 x = work.left + (work.right - work.left - w) / 2;
	i32 y = work.top + (work.bottom - work.top - h) / 2;

	hwnd = CreateWindowExW(0, wc.lpszClassName, L"client", style, x, y, w, h, 0, 0, wc.hInstance, 0);
	if (hwnd) {
		open = true;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	}
}

window_t::~window_t() {
	if (hwnd) {
		DestroyWindow(hwnd);
	}
}

bool window_t::pump() {
	MSG msg;

	while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return open;
}

LRESULT window_t::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (auto window = (window_t*)GetWindowLongPtrW(hwnd, GWLP_USERDATA)) {
		return window->handle_message(hwnd, msg, wparam, lparam);
	}

	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

LRESULT window_t::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_CLOSE: {
			DestroyWindow(hwnd);
		} return 0;

		case WM_DESTROY: {
			open = false;
			PostQuitMessage(0);
		} return 0;

		case WM_GETMINMAXINFO: {
			MINMAXINFO* mmi = (MINMAXINFO*)lparam;
			if (mmi) {
				DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
				RECT rect = { 0, 0, 128, 128 };
				AdjustWindowRect(&rect, style, FALSE);

				mmi->ptMinTrackSize.x = rect.right - rect.left;
				mmi->ptMinTrackSize.y = rect.bottom - rect.top;
			}
		} return 0;

		case WM_MENUCHAR: {
		} return MNC_CLOSE << 16;

		case WM_SIZE: {
			if (wparam != SIZE_MINIMIZED) {
				size_x = LOWORD(lparam);
				size_y = HIWORD(lparam);
			}
		} return 0;
	}

	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

}
