#include "sys_render.h"
#include "sys_com.h"
#include "sys_window.h"
#include "core_util.h"

namespace sys {

#include "shd_yuv_ps.h"
#include "shd_yuv_vs.h"

render_t::render_t(sys::window_t* window) {
	core::util::zero(this);

	this->window = window;

	if (window->hwnd == 0) {
		return;
	}

	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
		return;
	}

	if (FAILED(factory->EnumAdapters(0, &adapter))) {
		return;
	}

	if (FAILED(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, &device, 0, &context))) {
		return;
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;

	if (FAILED(factory->CreateSwapChainForHwnd(device, window->hwnd, &desc, 0, 0, &swap))) {
		return;
	}

	factory->MakeWindowAssociation(window->hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	ID3D11Debug* debug = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&debug)))) {
		ID3D11InfoQueue* iq = nullptr;
		if (SUCCEEDED(debug->QueryInterface(IID_PPV_ARGS(&iq)))) {
			iq->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			iq->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			iq->Release();
		}

		debug->Release();
	}

	create_d3d_bs(&bs_solid, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
	create_d3d_ds(&ds_disabled, FALSE, FALSE);
	create_d3d_rs(&rs_yuv, D3D11_CULL_NONE, D3D11_FILL_SOLID, FALSE);
	create_d3d_ss(&ss_linear, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	create_d3d_vs(&vs_yuv, bc_shd_yuv_vs);
	create_d3d_ps(&ps_yuv, bc_shd_yuv_ps);

	valid = reset();
}

render_t::~render_t() {
	if (swap) {
		swap->SetFullscreenState(FALSE, nullptr);
	}

	for (auto& plane : planes) {
		plane.destroy();
	}

	com::release(&ps_yuv);
	com::release(&vs_yuv);
	com::release(&ss_linear);
	com::release(&rs_yuv);
	com::release(&ds_disabled);
	com::release(&bs_solid);
	com::release(&backbuffer_rtv);
	com::release(&backbuffer_tex);
	com::release(&swap);
	com::release(&context);
	com::release(&device);
	com::release(&adapter);
	com::release(&factory);
}

bool render_t::frame_begin() {
	if (valid == false) {
		return false;
	}

	if (window->size_x && window->size_y) {
		if (backbuffer_w != window->size_x || backbuffer_h != window->size_y) {
			if (reset() == false) {
				valid = false;
				return false;
			}
		}
	}

	static const FLOAT clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(backbuffer_rtv, clear_color);
	context->OMSetRenderTargets(1, &backbuffer_rtv, nullptr);

	FLOAT const blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(bs_solid, blend_factor, 0xFFFFFFFF);

	context->OMSetDepthStencilState(ds_disabled, 0);
	context->RSSetState(rs_yuv);
	context->VSSetShader(vs_yuv, nullptr, 0);
	context->PSSetShader(ps_yuv, nullptr, 0);
	context->PSSetSamplers(0, 1, &ss_linear);
	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* srvs[3];
	srvs[0] = planes[0].srv;
	srvs[1] = planes[1].srv;
	srvs[2] = planes[2].srv;

	context->PSSetShaderResources(0, COUNT(srvs), srvs);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = backbuffer_w;
	viewport.Height = backbuffer_h;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	return true;
}

void render_t::frame_finish() {
	context->Draw(3, 0);

	HRESULT hr = swap->Present(1, 0);

	if (FAILED(hr) && hr != DXGI_STATUS_OCCLUDED && hr != DXGI_ERROR_WAS_STILL_DRAWING) {
		valid = false;
	}
}

void render_t::update_plane(u32 idx, void const* src, u32 stride, u32 w, u32 h) {
	if (idx < COUNT(planes)) {
		planes[idx].create(this, w, h);
		planes[idx].update(this, src, w, h, stride);
	}
}

bool render_t::reset() { 
	context->ClearState();

	com::release(&backbuffer_tex);
	com::release(&backbuffer_rtv);

	if (FAILED(swap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0))) {
		return false;
	}

	if (FAILED(swap->GetBuffer(0, IID_PPV_ARGS(&backbuffer_tex)))) {
		return false;
	}

	D3D11_TEXTURE2D_DESC bb_desc;
	backbuffer_tex->GetDesc(&bb_desc);

	backbuffer_w = bb_desc.Width;
	backbuffer_h = bb_desc.Height;

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
	rtv_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	if (FAILED(device->CreateRenderTargetView(backbuffer_tex, &rtv_desc, &backbuffer_rtv))) {
		return false;
	}

	return true;
}

void render_t::create_d3d_bs(ID3D11BlendState** out, D3D11_BLEND src_blend, D3D11_BLEND dst_blend) {
	D3D11_BLEND_DESC desc = {};

	desc.RenderTarget[0].BlendEnable = (src_blend != D3D11_BLEND_ONE) || (dst_blend != D3D11_BLEND_ZERO);
	desc.RenderTarget[0].SrcBlend = src_blend;
	desc.RenderTarget[0].SrcBlendAlpha = src_blend;
	desc.RenderTarget[0].DestBlend = dst_blend;
	desc.RenderTarget[0].DestBlendAlpha = dst_blend;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&desc, out);
}

void render_t::create_d3d_ds(ID3D11DepthStencilState** out, BOOL enable, BOOL write_enable) {
	D3D11_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = enable;
	desc.DepthWriteMask = write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace = desc.FrontFace;

	device->CreateDepthStencilState(&desc, out);
}

void render_t::create_d3d_rs(ID3D11RasterizerState** out, D3D11_CULL_MODE cull_mode, D3D11_FILL_MODE fill_mode, BOOL scissor) {
	D3D11_RASTERIZER_DESC desc = {};

	desc.CullMode = cull_mode;
	desc.FillMode = fill_mode;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = TRUE;
	desc.ScissorEnable = scissor;

	device->CreateRasterizerState(&desc, out);
}

void render_t::create_d3d_ss(ID3D11SamplerState** out, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode) {
	D3D11_SAMPLER_DESC desc = {};

	desc.Filter = filter;
	desc.AddressU = address_mode;
	desc.AddressV = address_mode;
	desc.AddressW = address_mode;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&desc, out);
}

void render_t::create_d3d_rtv(ID3D11RenderTargetView** out, ID3D11Texture2D* tex, DXGI_FORMAT format) {
	if (tex == nullptr) {
		*out = nullptr;
		return;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
	rtv_desc.Format = format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(tex, &rtv_desc, out);
}

void render_t::create_d3d_srv(ID3D11ShaderResourceView** out, ID3D11Texture2D* tex, DXGI_FORMAT format) {
	if (tex == nullptr) {
		*out = nullptr;
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(tex, &desc, out);
}

void render_t::create_d3d_tex(ID3D11Texture2D** out, u32 size_x, u32 size_y, DXGI_FORMAT format, D3D11_USAGE usage, UINT bind_flags, UINT cpu_flags) {
	if (size_x == 0 || size_y == 0) {
		return;
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = size_x;
	desc.Height = size_y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = usage;
	desc.BindFlags = bind_flags;
	desc.CPUAccessFlags = cpu_flags;
	device->CreateTexture2D(&desc, nullptr, out);
}

void render_t::texture_t::create(render_t* render, u32 size_x, u32 size_y) {
	if (this->size_x != size_x || this->size_y != size_y) {
		destroy();

		const DXGI_FORMAT format = DXGI_FORMAT_R8_UNORM;

		render->create_d3d_tex(&tex, size_x, size_y, format, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0);
		render->create_d3d_tex(&stage, size_x, size_y, format, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE);
		render->create_d3d_srv(&srv, tex, format);

		this->size_x = size_x;
		this->size_y = size_y;
	}
}

void render_t::texture_t::destroy() {
	com::release(&srv);
	com::release(&stage);
	com::release(&tex);
	size_x = 0;
	size_y = 0;
}


bool render_t::texture_t::lock(render_t* render, map_t* map) {
	if (stage == nullptr) {
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	if (FAILED(render->context->Map(stage, 0, D3D11_MAP_WRITE, 0, &ms))) {
		return false;
	}

	map->data = ms.pData;
	map->pitch = ms.RowPitch;

	return true;
}

void render_t::texture_t::unlock(render_t* render) {
	if (stage) {
		auto ctx = render->context;
		render->context->Unmap(stage, 0);
		render->context->CopyResource(tex, stage);
	}
}

void render_t::texture_t::update(render_t * render, void const * src, u32 size_x, u32 size_y, u32 stride) {
	map_t map;
	if (lock(render, &map)) {
		auto src_buf = (BYTE const*)src;
		auto dst_buf = (BYTE*)map.data;

		for (u32 y = 0; y < size_y; ++y) {
			__movsb(dst_buf, src_buf, size_x);
			src_buf += stride;
			dst_buf += map.pitch;
		}

		unlock(render);
	}
}

}