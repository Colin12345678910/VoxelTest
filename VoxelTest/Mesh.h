#pragma once
#include <SimpleMath.h>
#include <Effects.h>
#include <VertexTypes.h>
#include "Chunk.h"
#include <mutex>
using namespace DirectX;
using namespace DirectX::SimpleMath;
using DirectX::VertexPositionColor;

class Mesh
{
public:
	void CreateChunkMesh(Chunk* chunk, ID3D11Device* D3DDevice, ID3D11DeviceContext* context);
	void GenerateMesh(Chunk *chunk);
	void AddCubeMesh(Vector3 pos, bool up, bool down, bool left, bool right, bool front, bool back);
	void InitializeGeometry(ID3D11Device* D3DDevice);
	void InitalizeShaders(ID3D11Device* D3DDevice, ID3D11DeviceContext* context);
	void Draw(ID3D11DeviceContext* DeviceContext, const Matrix& view, const Matrix& world, const Matrix& projection);
	bool HasCompleted() { return hasCompleted; }
	void MarkDeletion() {
		MarkForDeletion = true;
		isDirty = false;
	}
	void MarkDirty() { 
		isDirty = true;
		hasCompleted = false;
	}
	Mesh();
	~Mesh();
protected:

	void ThreadMesh(Chunk* chunk, ID3D11Device* D3DDevice, ID3D11DeviceContext* context);
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndicesBuffer;
	ID3D11InputLayout* pInputLayout;
	BasicEffect* pEffect;

	int numVerts = 0;
	int numIndices = 0;
	std::list<VertexPositionColor> verts;
	std::list<UINT> indexs;
	bool hasCompleted = false;
	bool MarkForDeletion = false;
	VertexPositionColor* vertices;
	UINT* indices;
	bool isDirty = true;
	
	std::mutex mutex;
	
};

