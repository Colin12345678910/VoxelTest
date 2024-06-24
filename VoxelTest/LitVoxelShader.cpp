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
	pLitShaderPS = NULL;
	pLitShaderBlob = NULL;
	pLitShaderVS = NULL;
	pLitShaderBlobVS = NULL;
	pConstantBuffer = NULL;
}

void LitVoxelShader::LoadShader(ID3D11Device* device)
{
	//D3DWriteBlobToFile(pLitShaderBlob, L"..//Test", TRUE);
#ifdef NDEBUG
	HRESULT hr = D3DReadFileToBlob(L"..//x64//Release//LitVoxelShaderPS.cso", &pLitShaderBlob);
#else 
	HRESULT hr = D3DReadFileToBlob(L"..//x64//Debug//LitVoxelShaderPS.cso", &pLitShaderBlob);
#endif
	
	
	DX::ErrorIfFailed(hr, L"DX_MissingVoxelPS");

	hr = device->CreatePixelShader(pLitShaderBlob->GetBufferPointer(), pLitShaderBlob->GetBufferSize(), NULL, &pLitShaderPS);
	DX::ErrorIfFailed(hr, L"DX_CannotCreateVoxelPS");

#ifdef NDEBUG
	hr = D3DReadFileToBlob(L"..//x64//Release//LitVoxelShaderVS.cso", &pLitShaderBlobVS);
#else 
	hr = D3DReadFileToBlob(L"..//x64//Debug//LitVoxelShaderVS.cso", &pLitShaderBlobVS);
#endif


	DX::ErrorIfFailed(hr, L"DX_MissingVoxelVS");

	hr = device->CreateVertexShader(pLitShaderBlobVS->GetBufferPointer(), pLitShaderBlobVS->GetBufferSize(), NULL, &pLitShaderVS);
	DX::ErrorIfFailed(hr, L"DX_CannotCreateVoxelVS");
	
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

void LitVoxelShader::SetupShader(const Matrix& world, const Matrix& view, const Matrix& projection, ID3D11DeviceContext* pContext)
{
	ShaderConsts* shaderConsts;
	D3D11_MAPPED_SUBRESOURCE constantResource;

	//Get a pointer
	pContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantResource);
	shaderConsts = (ShaderConsts*)constantResource.pData; //Assign the shaderConsts class to the constantResource's data;

	shaderConsts->projMat = projection.Transpose();
	shaderConsts->viewMat = view.Transpose();
	shaderConsts->worldMat = world.Transpose();
	shaderConsts->worldViewProjMat = (world * view * projection).Transpose();

	pContext->Unmap(pConstantBuffer, 0);

	pContext->PSSetShader(pLitShaderPS, NULL, 0);
	pContext->VSSetShader(pLitShaderVS, NULL, 0);

	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
}

void LitVoxelShader::Release()
{
	pLitShaderPS->Release();
	pLitShaderBlob->Release();
	pConstantBuffer->Release();
}
