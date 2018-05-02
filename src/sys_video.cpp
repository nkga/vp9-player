#include "sys_video.h"
#include "sys_render.h"
#include "sys_timer.h"
#include "core_heap.h"
#include "core_util.h"
#include <vpx/vpx_image.h>
#include <stdlib.h>

namespace sys {

#pragma pack(push, 1)
struct ivf_header_t {
	u32 signature;
	u16 version;
	u16 length;
	u32 codec;
	u16 width;
	u16 height;
	u32 framerate;
	u32 timescale;
	u32 num_frames;
	u32 unused;
};
#pragma pack(pop)

video_player_t::video_player_t(render_t* render, timer_t* timer, i32 size_x, i32 size_y) {
	this->render = render;
	this->timer = timer;
	video_index = -1;

	vpx_codec_dec_cfg_t cfg = {};
	cfg.w = size_x;
	cfg.h = size_y;
	cfg.threads = 1;

	initialized = (vpx_codec_dec_init_ver(&codec, vpx_codec_vp9_dx(), &cfg, 0, VPX_DECODER_ABI_VERSION) == VPX_CODEC_OK);
}

video_player_t::~video_player_t() {
	if (initialized) {
		vpx_codec_destroy(&codec);
	}
}

void video_player_t::load(video_t* video_in) {
	if (initialized == false) {
		return;
	}

	if (video != video_in) {
		video = video_in;
		video_index = -1;
	}
}

void video_player_t::update(i64 us_time) {
	if (initialized == false || video == nullptr || video->frame_count < 2){
		return;
	}

	i64 us_looped = us_time % video->us_video_duration;
	i32 frame_index = (us_looped / video->us_frame_duration) % video->frame_count;

	if (video_index > frame_index) {
		video_index = -1;
		
		if (vpx_codec_decode(&codec, NULL, 0, NULL, NULL) != VPX_CODEC_OK) {
			return;
		}
	}

	i64 us_decode_start = timer->query();

	// force a first frame decode if the video is unloaded
	if (video_index < 0) {
		video_frame_t frame = video->frames[0];
		if (vpx_codec_decode(&codec, video->src + frame.pos, frame.bytes, NULL, 0) == VPX_CODEC_OK) {
			video_index = 0;
		}
	}

	// seek to nearest keyframe
	for (i32 index = video_index + 1; index < frame_index; ++index) {
		if (video->frames[index].kf) {
			video_index = index;
		}
	}

	// decode until we hit the loop stop point, or it takes too long
	while (video_index < frame_index) {
		i64 us_decode_now = timer->query();
		if (us_decode_now - us_decode_start >= 8000) {
			break;
		}

		video_frame_t frame = video->frames[video_index++];
		if (vpx_codec_decode(&codec, video->src + frame.pos, frame.bytes, NULL, 0) != VPX_CODEC_OK) {
			break;
		}
	}

	vpx_codec_iter_t iter = NULL;
	vpx_image_t* img = vpx_codec_get_frame(&codec, &iter);
	if (img && img->fmt == VPX_IMG_FMT_I420) {
		for (u32 i_plane = 0; i_plane < 3; ++i_plane) {
			i32 w = vpx_img_plane_width(img, i_plane);
			i32 h = vpx_img_plane_height(img, i_plane);
			render->update_plane(i_plane, img->planes[i_plane], img->stride[i_plane], w, h);
		}
		vpx_img_free(img);
	}
}

i32 video_player_t::vpx_img_plane_width(const vpx_image_t * img, i32 plane) {
	if (plane > 0 && img->x_chroma_shift > 0) {
		return (img->d_w + 1) >> img->x_chroma_shift;
	}

	return img->d_w;
}

i32 video_player_t::vpx_img_plane_height(const vpx_image_t * img, i32 plane) {
	if (plane > 0 && img->y_chroma_shift > 0) {
		return (img->d_h + 1) >> img->y_chroma_shift;
	}

	return img->d_h;
}

video_t::video_t(BYTE* src, u32 src_bytes) {
	core::util::zero(this);

	if (src == nullptr || src_bytes <= sizeof(ivf_header_t)) {
		return;
	}

	auto header = *(ivf_header_t*)src;

	u32 const signature = 0x30395056;

	if (header.codec != signature || header.framerate == 0 || header.num_frames == 0) {
		return;
	}

	this->frames = (video_frame_t*)calloc(header.num_frames, sizeof(video_frame_t));
	if (this->frames == nullptr) {
		return;
	}

	this->size_x = header.width;
	this->size_y = header.height;

	u32 num_frames = 0;
	u32 data_pos = sizeof(ivf_header_t);
	u32 data_end = src_bytes - 12;

	for (;;) {
		if (data_pos >= data_end) {
			break;
		}

		auto frame_bytes = *(u32*)(src + data_pos);
		if (data_pos + frame_bytes > src_bytes) {
			break;
		}

		data_pos += 12;

		auto frame = &this->frames[num_frames++];
		frame->pos = data_pos;
		frame->bytes = frame_bytes;

		vpx_codec_stream_info_t si;
		if (vpx_codec_peek_stream_info(&vpx_codec_vp9_dx_algo, src + data_pos, frame_bytes, &si) == VPX_CODEC_OK) {
			frame->kf = si.is_kf;
		}

		data_pos += frame_bytes;

		if (num_frames >= header.num_frames) {
			break;
		}
	}

	this->frame_count = num_frames;
	this->us_frame_duration = (1000000ll / header.framerate);
	this->us_video_duration = this->us_frame_duration * num_frames;
	this->src = src;
}

video_t::~video_t() {
	core::heap::raw_free(frames);
	core::heap::raw_free(src);
	core::util::zero(this);

}

}