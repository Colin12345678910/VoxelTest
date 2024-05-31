#include "pch.h"
#include "WorldRenderer.h" 
#include "World.h"

bool WorldRenderer::HasEnded = false;
void WorldRenderer::Render(const Matrix& view, const Matrix& world, const Matrix& projection)
{
	list<ChunkMesh> chunks = *World::GetChunks();
	chunks.begin();
	
	for (ChunkMesh chunk : chunks)
	{
		if (chunk.IsDirty())
		{

		}
		else
		{
			Matrix mat = mat.CreateTranslation(Vector3(chunk.chunk->GetChunkPos().x * Chunk::depth, 0, chunk.chunk->GetChunkPos().y * Chunk::width));
			chunk.mesh->Draw(Context, view, mat, projection);
		}
	}

	
}

void WorldRenderer::Initialize(ID3D11DeviceContext* DeviceContext, ID3D11Device* Device)
{
	D3DDevice = Device;
	Context = DeviceContext;
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		threadPool[i] = jthread(&WorldRenderer::MeshThread, this, i);
	}
	
}

void WorldRenderer::MeshThread(int threadNum)
{
	while (!HasEnded)
	{
		list<ChunkMesh> chunks = *World::GetChunks();
		for (ChunkMesh chunk : chunks)

			
			{
			
				//if (chunk.mesh->isDirty)
				//{
			if(!chunk.IsDirty())
					chunk.mesh->CreateChunkMesh(chunk.chunk, D3DDevice, Context);
				//}
			}
			
		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

int WorldRenderer::Modulo(int a, int b)
{
	return (b + (a % b)) % b;
}
