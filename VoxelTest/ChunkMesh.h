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
	bool IsDirty() 
	{
		if (isDeleted) return true;
		return isDirty;
	}
	bool Deleted() {
		return isDeleted;
	}
	
	void SetDeleted() { isDeleted = true; }
	void Generate(Noise& noise, Vector2 chunk);
private:
	bool isDirty = true;
	bool isDeleted = false;
};

