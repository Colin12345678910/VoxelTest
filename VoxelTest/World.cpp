#include "pch.h"
#include "World.h"
#include <iostream>
#include <string>

int World::RENDER_RADI = 8;
list<ChunkMesh> World::chunks;
bool World::isRunning = true;

World::World()
{

}
World::~World()
{

}
void World::Initalize()
{
	noise.Initialize();
	renderThread = std::jthread(&World::WorldGen, this);
}

void World::Update(int worldX, int worldY)
{
	x = worldX;
	y = worldY;

	for (ChunkMesh& mChunk : chunks)
	{
		//OutputDebugString(std::to_wstring(chunks.size()).c_str());
		//OutputDebugString(L"\n");
		
		if (mChunk.GetState() == ChunkMesh::Meshed)
		{
			mChunk.SetState(ChunkMesh::Finished);
		}
		if (mChunk.GetState() != ChunkMesh::Finished)
		{

		}
		else if (abs(mChunk.chunk->GetChunkPos().x - GetPlayerX()) > RENDER_RADI * .5 || abs(mChunk.chunk->GetChunkPos().y - GetPlayerY()) > RENDER_RADI * .5)
		{
			//m = &mChunk;
			//m.Release();
			if (chunks.size() <= 1)
			{
				return; //We don't want to deallocate all chunks as that fucks up the list structure.
			}
			

			if (mChunk.GetState() != ChunkMesh::Deallocated) //We do not, UNDER ANY CIRCUMSTANCE, want to deallocate memory that is already deallocated.
			{
				mChunk.Release();
				mChunk.SetState(ChunkMesh::Deallocated);
			}
				



			//break;
		}
	}
	//chunks.remove_if([](ChunkMesh c) { return c.GetState() == ChunkMesh::Deallocated; });
	
	if (chunks.size() > 64 * 2)
	{
		//throw std::out_of_range("Requested chunks exceed expected array length."); //Something seriously wrong has occured.
	}
	/*
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
			
			if( !mChunk.Deleted() ) //We do not, UNDER ANY CIRCUMSTANCE, want to deallocate memory that is already deallocated.wwwwwwwww
			mChunk.Release();
			
			
			
			//break;
		}
	}
	
	//chunks.remove_if([](ChunkMesh c) { return c.Deleted(); });
	//m.chunk->Release();
	//delete m.mesh;
	chunks.remove_if([](ChunkMesh c) { return c.Deleted() && c.mesh->HasCompleted(); });
	//chunks.remove_if([m](ChunkMesh c) { return c.chunk->GetChunkPos() == m.chunk->GetChunkPos(); });
	*/
}

	

void World::WorldGen()
{
	while (isRunning)
	{
	//OutputDebugStringW(L"[VoxelTest] Started Worldgen thread.");


	for (int x = -RENDER_RADI * 0.5; x < RENDER_RADI * 0.5 - 1; x++) {
		for (int y = -RENDER_RADI * 0.5; y < RENDER_RADI * 0.5 - 1; y++) {
			GenerateChunk(GetPlayerX() + x, GetPlayerY() + y, noise);
		}
	}





	//chunks.remove_if([](ChunkMesh c) { return c.Deleted(); });
	//m.chunk->Release();
	//delete m.mesh;
	
	//chunks.remove_if([m](ChunkMesh c) { return c.chunk->GetChunkPos() == m.chunk->GetChunkPos(); });


	this_thread::sleep_for(chrono::milliseconds(500));
	}
}

void World::GenerateChunk(int worldX, int worldY, Noise& noise)
{
	for (ChunkMesh mChunk : chunks)
	{
		if (mChunk.GetState() == ChunkMesh::Deallocated)
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
	
	for (ChunkMesh &mChunk : chunks)
	{
		if (mChunk.GetState() == ChunkMesh::Deallocated)
		{
			mChunk = newChunk;
			return;
		}
		
	}

	chunks.push_back(newChunk);


}
