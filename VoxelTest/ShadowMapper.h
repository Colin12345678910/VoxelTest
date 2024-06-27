#pragma once
#include "RenderTexture.h"
#include "DeviceResources.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;
class ShadowMapper

{
public:
	void Init(ID3D11Device* device);
	void RenderShadowMap(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilView* view);
	ID3D11ShaderResourceView* GetShaderResourceView() { return rt.GetStencilResourceView().Get(); }
	ID3D11ShaderResourceView** GetStencilShaderRV() { return rt.GetStencilResourceViewGetAddress(); }
	RenderTexture* GetRenderTexture() { return &rt; }
private:
	RenderTexture rt;
};

