#include "pch.h"
#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
}

void RenderTexture::Init(ID3D11Device* device, int x, int y, DXGI_FORMAT rtFormat, DXGI_FORMAT stencilFormat)
{
	d3dDevice = device; 
	UINT supported = 0;
	DX::ErrorIfFailed(device->CheckFormatSupport(rtFormat, &supported), L"Invalid Rendertexture format.");
	
	CD3D11_TEXTURE2D_DESC rtDesc(
		rtFormat,
		x,
		y,
		1,
		1,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		D3D11_USAGE_DEFAULT
	);

	DX::ErrorIfFailed(device->CreateTexture2D(
		&rtDesc,
		nullptr,
		renderTarget.ReleaseAndGetAddressOf()
	),
		L"Invalid rendertexture");

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		stencilFormat,
		x,
		y,
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	DX::ThrowIfFailed(device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		depthStencil.ReleaseAndGetAddressOf()
	));

	DX::ThrowIfFailed(device->CreateDepthStencilView(
		depthStencil.Get(),
		nullptr,
		depthStencilView.ReleaseAndGetAddressOf()
	));

	CD3D11_RENDER_TARGET_VIEW_DESC rtTargetViewDesc(
		D3D11_RTV_DIMENSION_TEXTURE2D, rtFormat
	);

	DX::ErrorIfFailed(device->CreateRenderTargetView(
		renderTarget.Get(),
		&rtTargetViewDesc,
		&rTView
	),
		L"Invalid RT view");

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc(
		D3D11_SRV_DIMENSION_TEXTURE2D, rtFormat
	);

	DX::ErrorIfFailed(device->CreateShaderResourceView(
		renderTarget.Get(),
		&shaderResourceDesc,
		m_shaderResourceView.ReleaseAndGetAddressOf()
	),
		L"Invalid ShaderResourceView");
}

void RenderTexture::Release()
{
	d3dDevice = NULL;
	renderTarget.ReleaseAndGetAddressOf();
	rTView.ReleaseAndGetAddressOf();
	m_shaderResourceView.ReleaseAndGetAddressOf();
}
