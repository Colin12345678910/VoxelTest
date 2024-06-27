#pragma once
class RenderTexture
{
public:
	RenderTexture();
	void Init(ID3D11Device* device, int x, int y, DXGI_FORMAT format, DXGI_FORMAT stencilFormat);
	void Release();
	Microsoft::WRL::ComPtr<ID3D11Texture2D> GetRenderTarget() { return renderTarget; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() { return rTView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetStencilTargetView() { return depthStencilView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return m_shaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetStencilResourceView() { return depthResourceView; }
	ID3D11ShaderResourceView** GetStencilResourceViewGetAddress() { return depthResourceView.GetAddressOf(); }
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rTView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthResourceView;
	ID3D11Device* d3dDevice;
};

