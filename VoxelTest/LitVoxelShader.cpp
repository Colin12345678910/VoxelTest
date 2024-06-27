#include "pch.h"
#include "LitVoxelShader.h"
#include <d3dcompiler.h>

struct ShaderConsts
{
	Matrix worldMat;
	Matrix viewMat;
	Matrix projMat;
	Matrix worldViewProjMat;
	Matrix shadowCOORD;
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
	hr = D3DReadFileToBlob(L"..//x64//Release//UnLitVoxelShaderPS.cso", &pUnLitShaderBlob);
#else 
	hr = D3DReadFileToBlob(L"..//x64//Debug//UnLitVoxelShaderPS.cso", &pUnLitShaderBlob);
#endif


	DX::ErrorIfFailed(hr, L"DX_MissingUnlitVoxelPS");

	hr = device->CreatePixelShader(pUnLitShaderBlob->GetBufferPointer(), pUnLitShaderBlob->GetBufferSize(), NULL, &pUnLitShaderPS);
	DX::ErrorIfFailed(hr, L"DX_CannotCreateUnlitVoxelPS");

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

void LitVoxelShader::SetupShader(const Matrix& world, const Matrix& view, const Matrix& projection, const Matrix& shadowCoord, ID3D11DeviceContext* Context)
{
	SetupShader(world, view, projection, shadowCoord, Context, false);
}

void LitVoxelShader::SetupShader(const Matrix& world, const Matrix& view, const Matrix& projection, const Matrix& shadowCoord, ID3D11DeviceContext* pContext, bool isUnlit)
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
	shaderConsts->shadowCOORD = (world * shadowCoord).Transpose();

	pContext->Unmap(pConstantBuffer, 0);

	if (isUnlit)
	{
		pContext->PSSetShader(pUnLitShaderPS, NULL, 0);
	}
	else {
		pContext->PSSetShader(pLitShaderPS, NULL, 0);
	}
	
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
