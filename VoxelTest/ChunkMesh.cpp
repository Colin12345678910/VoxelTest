#include "pch.h"
#include "ChunkMesh.h"

ChunkMesh::ChunkMesh()
{
	ChunkState::Empty;
	chunk = new Chunk();
	mesh = new Mesh();
}

ChunkMesh::~ChunkMesh()
{

	
}

void ChunkMesh::Release()
{
	currentState = ChunkState::Deallocated;
	mesh->MarkDeletion();
	//delete chunk;
	Chunk* pChunk; //Save the prev pointer
	pChunk = chunk; 
	chunk = NULL; //Set the chunk to NULL
	pChunk->Release(); //Release Memory
	delete pChunk;
	delete mesh;
}

void ChunkMesh::Generate(Noise& noise, Vector2 chunkPos)
{
	chunk->FillBlocks(noise, chunkPos);
	currentState = ChunkState::Placed;
}
