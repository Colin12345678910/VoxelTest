#pragma once
#include <SimpleMath.h>
#include <d3d11.h>


using DirectX::SimpleMath::Matrix;
class LitVoxelShader
{
public:
	LitVoxelShader();
	void LoadShader(ID3D11Device* device);
	void SetupShader(const Matrix &world, const Matrix &view, const Matrix &projection, ID3D11DeviceContext* Context);
	ID3D11PixelShader* Get() { return pLitShaderPS; }
	void Release();
	const void* GetShaderBinPS() {
		return pLitShaderBlob->GetBufferPointer();
	}
	const void* GetShaderBinVS() {
		return pLitShaderBlobVS->GetBufferPointer();
	}
	size_t GetSizePS() { return pLitShaderBlob->GetBufferSize(); };
	size_t GetSizeVS() { return pLitShaderBlobVS->GetBufferSize(); };
private:
	ID3DBlob* pLitShaderBlob;
	ID3DBlob* pLitShaderBlobVS;
	ID3D11PixelShader* pLitShaderPS;
	ID3D11VertexShader* pLitShaderVS;
	ID3D11Buffer* pConstantBuffer;
};

