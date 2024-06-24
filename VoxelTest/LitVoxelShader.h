#pragma once
#include <SimpleMath.h>
#include <d3d11.h>


using DirectX::SimpleMath::Matrix;
class LitVoxelShader
{
public:
	LitVoxelShader();
	void LoadShader(ID3D11Device* device);
	void SetupShader();
	ID3D11PixelShader* Get() { return pLitShader; }
	void Release();
	const void* GetShaderBin() {
		return pLitShaderBlob;
	}
	size_t GetSize();
private:
	ID3DBlob* pLitShaderBlob;
	ID3D11PixelShader* pLitShader;
	ID3D11Buffer* pConstantBuffer;
};

