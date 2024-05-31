#include "pch.h"
#include "ChunkMesh.h"

ChunkMesh::ChunkMesh()
{
	isDirty = true;
	chunk = new Chunk();
	mesh = new Mesh();
}

ChunkMesh::~ChunkMesh()
{

	
}

void ChunkMesh::Release()
{
	isDeleted = true;
	mesh->MarkDeletion();
	//delete chunk;
	Chunk* pChunk; //Save the prev pointer
	pChunk = chunk; 
	chunk = NULL; //Set the chunk to NULL
	pChunk->Release(); //Release Memory
	delete pChunk;
	
	
	if (mesh->HasCompleted())
	{
		delete mesh;
	}
}

void ChunkMesh::Generate(Noise& noise, Vector2 chunkPos)
{
	chunk->FillBlocks(noise, chunkPos);
	isDirty = false;
}
