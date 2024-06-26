#include "pch.h"
#include "ShadowMapper.h"

void ShadowMapper::Init(ID3D11Device* device)
{
	
	rt.Init(device, 8192, 8192, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);

	
}

void ShadowMapper::RenderShadowMap(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilView* view)
{
	
	context->OMSetRenderTargets(1, rt.GetRenderTargetView().GetAddressOf(), rt.GetStencilTargetView().Get());
	Viewport vp(0, 0, 8192, 8192);
	context->RSSetViewports(1, vp.Get11());
	context->ClearDepthStencilView(rt.GetStencilTargetView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->ClearRenderTargetView(rt.GetRenderTargetView().Get(), Color(0.0f, 0.0f, 0.0f));
}