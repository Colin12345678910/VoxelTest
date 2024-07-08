#pragma once
#include <SimpleMath.h>
#include <Effects.h>
#include <VertexTypes.h>
#include <DirectXCollision.h>
#include "ChunkMesh.h"
#include "Chunk.h"
#include "LitVoxelShader.h"


#include <Thread>
using namespace DirectX;
using namespace DirectX::SimpleMath;
using DirectX::VertexPositionColor;
class WorldRenderer

{
public:
	void Render(const Matrix& view, const Matrix& world, const Matrix& projection, const Matrix& shadowCoord, bool isLit);
	void Initialize(ID3D11DeviceContext* DeviceContext, ID3D11Device* Device, LitVoxelShader* litVoxelS);
	static void End() { HasEnded = true; }
private:
	static const int THREAD_COUNT = 12;
	static bool HasEnded;
	BoundingFrustum frustrum;
	BoundingBox box;
	std::jthread threadPool[THREAD_COUNT];
	void MeshThread(int threadNum);
	int Modulo(int a, int b);
	ID3D11DeviceContext* Context;
	ID3D11Device* D3DDevice;
	LitVoxelShader* litVoxelShader;
};

