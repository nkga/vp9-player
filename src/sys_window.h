#pragma once
#include "core.h"
#include <Windows.h>

namespace sys {

struct render_t;

struct window_t {
	friend render_t;

	window_t();
	~window_t();

	bool pump();

protected:
	static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND hwnd;
	i32 size_x;
	i32 size_y;
	bool open;
};

}
