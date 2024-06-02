#pragma once
#include "Mesh.h"
#include "Chunk.h"
#include "Noise.h"

using namespace std;
class ChunkMesh
{
public:
	ChunkMesh();
	~ChunkMesh();
	Mesh* mesh;
	Chunk* chunk;
	void Release();
	void Generate(Noise& noise, Vector2 chunk);
	
	
	enum ChunkState
	{
		Empty,
		Placed,
		Meshed,
		Finished,
		Deallocated
	};
	ChunkState GetState() { return currentState; }
	void SetState(ChunkState state) { currentState = state; }
private:
	ChunkState currentState;
};

