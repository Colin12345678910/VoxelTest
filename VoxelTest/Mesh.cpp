#include "pch.h"
#include "Mesh.h"
#include "DeviceResources.h"
#include <list>
#include <thread>
#include <iostream>
using namespace std;



int num_blocks = 16;



bool Mesh::CreateChunkMesh(Chunk* chunk, ID3D11Device* D3DDevice, ID3D11DeviceContext* context, LitVoxelShader* litShader)
{
	//std::jthread t(&Mesh::ThreadMesh, this, chunk, D3DDevice, context);
	//if (MarkForDeletion) return;
	//if (!isDirty) return;
	if (!mutex.try_lock()) return false;
	GenerateMesh(chunk);
	InitalizeShaders(D3DDevice, context, litShader);
	InitializeGeometry(D3DDevice);
	isDirty = false;
	
	mutex.unlock();
	if (MarkForDeletion) {
		delete this;
		return false;
	}
	return true;
} 

void Mesh::GenerateMesh(Chunk* chunk)
{
	Chunk stackChunk = *chunk;
	int b;
	bool up, down, left, right, front, back;
	

	for (int x = 0; x < Chunk::width; x++) {
		for (int z = 0; z < Chunk::depth; z++) {
			for (int y = 0; y < Chunk::height; y++) {

				if (!chunk->IsDirty())
				{
					
					b = stackChunk.GetBlock(Vector3(x, y, z), up, down, left, right, front, back);
					if (b == 1) {
						AddCubeMesh(Vector3(x, y, z), up, down, left, right, front, back);
					}
				}
				
				
			}
		}
	}
	numVerts = numIndices = 0;

	//delete vertices;
	vertices = new VertexPositionColor[verts.size()];
	int Vertsize = verts.size();
	//for (int i = 0; i < Vertsize; i++) {
	//	vertices[i] = verts.front();
	//	verts.pop_front();
	//	numVerts++;
	//}
	while (!verts.empty())
	{
		vertices[numVerts] = verts.front();
		verts.pop_front();
		//verts.erase(verts.begin());
		numVerts++;
	}

	verts.clear();
	
	//delete indices;
	indices = new UINT[indexs.size()];
	int indexSize = indexs.size();

	while (!indexs.empty())
	{
		indices[numIndices] = indexs.front();
		indexs.pop_front();
		//indexs.erase(indexs.begin());
		numIndices++;
	}

	indexs.clear();
	
}

void Mesh::AddCubeMesh(Vector3 pos, bool up, bool down, bool left, bool right, bool front, bool back)
{
	//Front
	int offset;
	if (back) {
		offset = verts.size();
		verts.push_back({ Vector3(0.0f, 1.0f, 1.0f) + pos, Colors::Green.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 1.0f) + pos, Colors::Green.v });
		verts.push_back({ Vector3(0.0f, 0.0f, 1.0f) + pos, Colors::Green.v });
		verts.push_back({ Vector3(1.0f, 1.0f, 1.0f) + pos, Colors::Green.v });

		indexs.push_back(0 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(2 + offset);
		indexs.push_back(0 + offset);
		indexs.push_back(3 + offset);
		indexs.push_back(1 + offset);
	}
	
	//Back
	if (front) {
		offset = verts.size();
		verts.push_back({ Vector3(0.0f, 1.0f, 0.0f) + pos, Colors::Purple.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 0.0f) + pos, Colors::Purple.v });
		verts.push_back({ Vector3(0.0f, 0.0f, 0.0f) + pos, Colors::Purple.v });
		verts.push_back({ Vector3(1.0f, 1.0f, 0.0f) + pos, Colors::Purple.v });

		indexs.push_back(2 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(0 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(3 + offset);
		indexs.push_back(0 + offset);
	}
	
	//Left side
	if (left) {
		offset = verts.size();
		verts.push_back({ Vector3(0.0f, 0.0f, 0.0f) + pos, Colors::Red.v });
		verts.push_back({ Vector3(0.0f, 1.0f, 1.0f) + pos, Colors::Red.v });
		verts.push_back({ Vector3(0.0f, 0.0f, 1.0f) + pos, Colors::Red.v });
		verts.push_back({ Vector3(0.0f, 1.0f, 0.0f) + pos, Colors::Red.v });

		indexs.push_back(0 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(2 + offset);
		indexs.push_back(0 + offset);
		indexs.push_back(3 + offset);
		indexs.push_back(1 + offset);
	}

	//right side
	if (right)
	{
		offset = verts.size();
		verts.push_back({ Vector3(1.0f, 1.0f, 0.0f) + pos, Colors::Yellow.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 1.0f) + pos, Colors::Yellow.v });
		verts.push_back({ Vector3(1.0f, 1.0f, 1.0f) + pos, Colors::Yellow.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 0.0f) + pos, Colors::Yellow.v });

		indexs.push_back(0 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(2 + offset);
		indexs.push_back(0 + offset);
		indexs.push_back(3 + offset);
		indexs.push_back(1 + offset);
	}
	
	//up side
	if (up)
	{
		offset = verts.size();
		verts.push_back({ Vector3(0.0f, 1.0f, 0.0f) + pos, Color(pos.y, pos.y, pos.y)});
		verts.push_back({ Vector3(0.0f, 1.0f, 1.0f) + pos, Color(pos.y, pos.y, pos.y) });
		verts.push_back({ Vector3(1.0f, 1.0f, 0.0f) + pos, Color(pos.y, pos.y, pos.y) });
		verts.push_back({ Vector3(1.0f, 1.0f, 1.0f) + pos, Color(pos.y, pos.y, pos.y) });

		indexs.push_back(2 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(0 + offset);
		indexs.push_back(3 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(2 + offset);
	}
	

	//Bottom side
	if (down)
	{
		offset = verts.size();
		verts.push_back({ Vector3(0.0f, 0.0f, 0.0f) + pos, Colors::DarkKhaki.v });
		verts.push_back({ Vector3(0.0f, 0.0f, 1.0f) + pos, Colors::DarkKhaki.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 0.0f) + pos, Colors::DarkKhaki.v });
		verts.push_back({ Vector3(1.0f, 0.0f, 1.0f) + pos, Colors::DarkKhaki.v });

		indexs.push_back(0 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(2 + offset);
		indexs.push_back(2 + offset);
		indexs.push_back(1 + offset);
		indexs.push_back(3 + offset);
	}
}

void Mesh::InitializeGeometry(ID3D11Device* D3DDevice)
{
	D3D11_BUFFER_DESC geometryBuffer;
	geometryBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	geometryBuffer.CPUAccessFlags = 0;
	geometryBuffer.MiscFlags = 0;
	geometryBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	geometryBuffer.ByteWidth = numVerts * sizeof(VertexPositionColor);

	//Setup data
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	if (numVerts == 0) {
		return;
	}
	DX::ThrowIfFailed(D3DDevice->CreateBuffer(&geometryBuffer, &data, &pVertexBuffer));

	D3D11_BUFFER_DESC indicesBuffer;
	indicesBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indicesBuffer.CPUAccessFlags = 0;
	indicesBuffer.MiscFlags = 0;
	indicesBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indicesBuffer.ByteWidth = numIndices * sizeof(UINT);

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;


	if (numIndices == 0) {
		return;
	}
	DX::ThrowIfFailed(D3DDevice->CreateBuffer(&indicesBuffer, &indexData, &pIndicesBuffer));
}

void Mesh::InitalizeShaders(ID3D11Device* D3DDevice, ID3D11DeviceContext* context, LitVoxelShader* litShader)
{
	//pEffect = new BasicEffect(D3DDevice);
	//pEffect->SetLightingEnabled(false);
	//pEffect->SetTextureEnabled(false);
	//pEffect->SetVertexColorEnabled(true);
	//
	//D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
	//	{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	//void const* shaderByteCode;
	//size_t byteCodeLength;

	//pEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(D3DDevice->CreateInputLayout(
		VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		litShader->GetShaderBinVS(), litShader->GetSizeVS(),
		&pInputLayout
	));

	
}

void Mesh::Draw(ID3D11DeviceContext* DeviceContext, const Matrix& view, const Matrix& world, const Matrix& projection, LitVoxelShader* litShader)
{
	litShader->SetupShader(world, view, projection, DeviceContext);

	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceContext->IASetInputLayout(pInputLayout);

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(pIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	DeviceContext->DrawIndexed(numIndices, 0, 0);
}

Mesh::Mesh()
{
	pVertexBuffer = 0;
	pInputLayout = 0;
	pIndicesBuffer = 0;
	pEffect = 0;
	numVerts = 0;
	numIndices = 0;
	vertices = 0;
	indices = 0;
}

Mesh::~Mesh()
{
	if(pVertexBuffer != nullptr)
		pVertexBuffer->Release();
	if (pIndicesBuffer != nullptr)
		pIndicesBuffer->Release();
	if (pInputLayout != nullptr)
		pInputLayout->Release();
	delete indices;
	delete pEffect;
	delete vertices;
}

void Mesh::Release()
{
	if (pVertexBuffer != nullptr)
		pVertexBuffer->Release();
	if (pIndicesBuffer != nullptr)
		pIndicesBuffer->Release();
	if (pInputLayout != nullptr)
		pInputLayout->Release();
	delete indices;
	delete pEffect;
	delete vertices;
}
