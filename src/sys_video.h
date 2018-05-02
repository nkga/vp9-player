#pragma once
#include "core.h"
#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

namespace sys {

struct render_t;
struct timer_t;

struct video_frame_t {
	u32 pos;
	u32 bytes;
	u32 kf;
};

struct video_t {
	video_t(BYTE* src, u32 bytes);
	~video_t();

	BYTE* src;
	video_frame_t* frames;
	u32 frame_count;
	u32 us_frame_duration;
	u32 us_video_duration;
	i32 size_x;
	i32 size_y;
};

struct video_player_t {
	video_player_t(render_t* render, timer_t* timer, i32 size_x, i32 size_y);
	~video_player_t();

	void load(video_t* video_in);
	void update(i64 us_time);

protected:
	static i32 vpx_img_plane_width(const vpx_image_t *img, i32 plane);
	static i32 vpx_img_plane_height(const vpx_image_t *img, i32 plane);

	render_t* render;
	timer_t* timer;
	vpx_codec_ctx_t codec;
	video_t* video;
	i32 video_index;
	bool initialized;
};

}
