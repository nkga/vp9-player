#include "core_fs.h"
#include "sys_dialog.h"
#include "sys_render.h"
#include "sys_timer.h"
#include "sys_video.h"
#include "sys_window.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE inst, HINSTANCE previnst, LPSTR cmdline, int cmdshow) {
	// Initialize subsystems.
	sys::timer_t timer;
	sys::window_t window;
	sys::render_t render(&window);

	// Load the video file.
	sys::dialog::startup();
	WCHAR* path = sys::dialog::open();

	u32 src_bytes;
	BYTE* src = (BYTE*)core::fs::read(path, &src_bytes);
	sys::video_t video(src, src_bytes);
	
	sys::video_player_t player(&render, &timer, video.size_x, video.size_y);
	player.load(&video);

	// Play the video.
	i64 us_start = timer.query();

	while (window.pump()) {
		if (render.frame_begin()) {
			player.update(timer.query() - us_start);
			render.frame_finish();
		}
	}

	return 0;
}
