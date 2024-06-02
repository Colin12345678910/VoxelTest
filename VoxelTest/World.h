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
	static void ToggleWorldGen() { isRunning = !isRunning; }
private:
	static list<ChunkMesh> chunks;
	static bool isRunning;
	int x;
	int y;
	Noise noise;
	int GetPlayerX() { return x; }
	int GetPlayerY() { return y; }
	void WorldGen();
	jthread renderThread;
	void GenerateChunk(int worldX, int worldY, Noise& noise);
};

