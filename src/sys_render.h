#pragma once
#include "core.h"
#include <Windows.h>
#include <d3d11_2.h>

namespace sys {

struct window_t;

struct render_t {
	render_t(sys::window_t* window);
	~render_t();

	bool frame_begin();
	void frame_finish();
	void update_plane(u32 idx, void const* src, u32 stride, u32 w, u32 h);

protected:
	struct map_t {
		void* data;
		u32 pitch;
	};

	struct texture_t {
		ID3D11Texture2D* tex;
		ID3D11Texture2D* stage;
		ID3D11ShaderResourceView* srv;
		u32 size_x;
		u32 size_y;

		void create(render_t* render, u32 size_x, u32 size_y);
		void destroy();
		bool lock(render_t* render, map_t* map);
		void unlock(render_t* render);
		void update(render_t* render, void const* src, u32 size_x, u32 size_y, u32 stride);
	};

	bool reset();

	void create_d3d_bs(ID3D11BlendState** out, D3D11_BLEND src_blend, D3D11_BLEND dst_blend);
	void create_d3d_ds(ID3D11DepthStencilState** out, BOOL enable, BOOL write_enable);
	void create_d3d_rs(ID3D11RasterizerState** out, D3D11_CULL_MODE cull_mode, D3D11_FILL_MODE fill_mode, BOOL scissor);
	void create_d3d_ss(ID3D11SamplerState** out, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode);
	void create_d3d_rtv(ID3D11RenderTargetView** out, ID3D11Texture2D* tex, DXGI_FORMAT format);
	void create_d3d_srv(ID3D11ShaderResourceView** out, ID3D11Texture2D* tex, DXGI_FORMAT format);
	void create_d3d_tex(ID3D11Texture2D** out, u32 size_x, u32 size_y, DXGI_FORMAT format, D3D11_USAGE usage, UINT bind_flags, UINT cpu_flags);
	
	sys::window_t* window;
	IDXGIFactory2* factory;
	IDXGIAdapter* adapter;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain1* swap;
	ID3D11Texture2D* backbuffer_tex;
	ID3D11RenderTargetView* backbuffer_rtv;
	ID3D11BlendState* bs_solid;
	ID3D11DepthStencilState* ds_disabled;
	ID3D11RasterizerState* rs_yuv;
	ID3D11SamplerState* ss_linear;
	ID3D11VertexShader* vs_yuv;
	ID3D11PixelShader* ps_yuv;
	texture_t planes[3];
	u32 backbuffer_w;
	u32 backbuffer_h;
	BOOL valid;

	template <u32 bc_size>
	void create_d3d_ps(ID3D11PixelShader** out, BYTE const (&bc_ps)[bc_size]) {
		device->CreatePixelShader(bc_ps, bc_size, nullptr, out);
	}

	template <u32 bc_size>
	void create_d3d_vs(ID3D11VertexShader** out, BYTE const (&bc_vs)[bc_size]) {
		device->CreateVertexShader(bc_vs, bc_size, nullptr, out);
	}
};

}
