#include "pch.h"
#include "World.h"

int World::RENDER_RADI = 48;
list<ChunkMesh> World::chunks;

World::World()
{

}
World::~World()
{

}
void World::Initalize()
{
	noise.Initialize();
	std::jthread(&World::WorldGen, this);
}

void World::Update(int worldX, int worldY)
{

	for (int x = -RENDER_RADI * 0.5; x < RENDER_RADI * 0.5 - 1; x++) {
		for (int y = -RENDER_RADI * 0.5; y < RENDER_RADI * 0.5 - 1; y++) {
			GenerateChunk(worldX + x, worldY + y, noise);
		}
	}

	

	for (ChunkMesh &mChunk : chunks)
	{
		if (mChunk.Deleted())
		{

		}
		else if (abs(mChunk.chunk->GetChunkPos().x - worldX) > RENDER_RADI * .5 || abs(mChunk.chunk->GetChunkPos().y - worldY) > RENDER_RADI * .5 && mChunk.mesh->HasCompleted())
		{
			//m = &mChunk;
			//m.Release();
			if (chunks.size() <= 1)
			{
				return; //We don't want to deallocate all chunks as that fucks up the list structure.
			}
			//mChunk.SetDeleted();
			//chunks.remove_if([mChunk](ChunkMesh c) { return c.chunk->GetChunkPos() == mChunk.chunk->GetChunkPos(); });
			
			if( !mChunk.Deleted() ) //We do not, UNDER ANY CIRCUMSTANCE, want to deallocate memory that is already deallocated.
			mChunk.Release();
			
			
			
			//break;
		}
	}
	
	//chunks.remove_if([](ChunkMesh c) { return c.Deleted(); });
	//m.chunk->Release();
	//delete m.mesh;
	chunks.remove_if([](ChunkMesh c) { return c.Deleted() && c.mesh->HasCompleted(); });
	//chunks.remove_if([m](ChunkMesh c) { return c.chunk->GetChunkPos() == m.chunk->GetChunkPos(); });
	
}

	

void World::WorldGen()
{
}

void World::GenerateChunk(int worldX, int worldY, Noise& noise)
{
	for (ChunkMesh mChunk : chunks)
	{
		if (mChunk.Deleted())
		{

		}
		else if (mChunk.chunk->GetChunkPos() == Vector2(worldX, worldY))
		{
			return;
		}
	}
	//Then we aren't in those chunks.
	ChunkMesh newChunk;
	newChunk.Generate(noise, Vector2(worldX,worldY));
	chunks.push_back(newChunk);

}
