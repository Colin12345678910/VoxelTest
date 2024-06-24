#include "pch.h"
#include "LitVoxelShader.h"
#include <d3dcompiler.h>

struct ShaderConsts
{
	Matrix worldMat;
	Matrix viewMat;
	Matrix projMat;
	Matrix worldViewProjMat;
};

LitVoxelShader::LitVoxelShader()
{
	pLitShader = NULL;
	pLitShaderBlob = NULL;
	pConstantBuffer = NULL;
}

void LitVoxelShader::LoadShader(ID3D11Device* device)
{
	//D3DWriteBlobToFile(pLitShaderBlob, L"..//Test", TRUE);
	HRESULT hr = D3DReadFileToBlob(L"..//x64//Release//LitVoxelShaderPS.cso", &pLitShaderBlob);
	
	DX::ErrorIfFailed(hr, L"DX_MissingVoxelPS");

	hr = device->CreatePixelShader(pLitShaderBlob->GetBufferPointer(), pLitShaderBlob->GetBufferSize(), NULL, &pLitShader);
	DX::ErrorIfFailed(hr, L"DX_CannotCreateVoxelPS");
	
	ShaderConsts init;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(ShaderConsts);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &init;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&bufferDesc, &data, &pConstantBuffer);
	DX::ErrorIfFailed(hr, L"DX_CannotCreateBuffer");
}

void LitVoxelShader::Release()
{
	pLitShader->Release();
	pLitShaderBlob->Release();
	pConstantBuffer->Release();
}
