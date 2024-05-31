#pragma once
#include "Chunk.h"
#include "ChunkMesh.h"
#include "Noise.h"
#include <thread>
using namespace std;
class World
{
	
public:
	World();
	~World();
	static int RENDER_RADI;
	void Initalize();
	void Update(int worldX, int worldY);
	static list<ChunkMesh>* GetChunks() { return &chunks; }
private:
	static list<ChunkMesh> chunks;
	int x;
	int y;
	Noise noise;
	int GetPlayerX() { return x; }
	int GetPlayerY() { return y; }
	void WorldGen();
	jthread renderThread;
	void GenerateChunk(int worldX, int worldY, Noise& noise);
};

