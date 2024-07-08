#include "pch.h"
#include "WorldRenderer.h" 
#include "World.h"

bool WorldRenderer::HasEnded = false;
void WorldRenderer::Render(const Matrix& view, const Matrix& world, const Matrix& projection, const Matrix& shadowCoord, bool isLit)
{
	list<ChunkMesh> chunks = *World::GetChunks();
	//chunks.begin();
	
	for (ChunkMesh chunk : chunks)
	{
		if (chunk.GetState() != chunk.Finished)
		{

		}
		else
		{
			
			
			BoundingFrustum::CreateFromMatrix(frustrum, projection, true);
			frustrum.Transform(frustrum, view.Invert());

			box.CreateFromPoints(box, Vector3(chunk.chunk->GetChunkPos().x * 32, 0, chunk.chunk->GetChunkPos().y * 32), Vector3(chunk.chunk->GetChunkPos().x * 32 + 32, Chunk::height, chunk.chunk->GetChunkPos().y * 32 + 32));
				
			if (!isLit || frustrum.Contains(box))
			{
				
				Matrix mat = mat.CreateTranslation(Vector3(chunk.chunk->GetChunkPos().x * Chunk::depth, 0, chunk.chunk->GetChunkPos().y * Chunk::width));
				chunk.mesh->Draw(Context, view, mat, projection, shadowCoord, litVoxelShader, isLit);
			}

			
		}
	}

	
}

void WorldRenderer::Initialize(ID3D11DeviceContext* DeviceContext, ID3D11Device* Device, LitVoxelShader* litVoxelS)
{
	D3DDevice = Device;
	Context = DeviceContext;
	litVoxelShader = litVoxelS;

	
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		threadPool[i] = jthread(&WorldRenderer::MeshThread, this, i);
	}
	
}

void WorldRenderer::MeshThread(int threadNum)
{
	while (!HasEnded)
	{
		//list<ChunkMesh> chunks = *World::GetChunks();
		
		list<ChunkMesh>::iterator iter;
		iter = World::GetChunks()->begin();

		while (iter != World::GetChunks()->end())
		{
			if (iter->GetState() == ChunkMesh::Placed)
			{
				if (iter->mesh->CreateChunkMesh(iter->chunk, D3DDevice, Context, litVoxelShader))
				{
					iter->SetState(ChunkMesh::Meshed);
				}
				
			}	
			iter++;
		}

			
		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

int WorldRenderer::Modulo(int a, int b)
{
	return (b + (a % b)) % b;
}
