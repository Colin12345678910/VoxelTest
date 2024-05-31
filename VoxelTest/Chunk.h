#pragma once
#include <SimpleMath.h>
#include "Noise.h"

using namespace DirectX::SimpleMath;
class Chunk
{
public:
	static const int width = 32;
	static const int depth = 32;
	static const int height = 256;

	static const int NUM_BLOCKS = width * depth * height;

	USHORT* blocks;
	bool* isEmpty;
	void Release();
	Chunk();
	~Chunk();
	void FillBlocks(Noise &noise, Vector2 chunk);
	int GetBlock(Vector3 pos);
	int GetBlock(Vector3 pos, bool &up, bool &down, bool &left, bool &right, bool &front, bool &back);
	void SetBlock(int x, int y, int z, int block);
	int ConvertPosToIndex(Vector3 pos);
	bool IsDirty() { return isDirty; }
	Vector2 GetChunkPos() { return Vector2(worldXPos, worldYPos); }
private:
	int worldXPos;
	int worldYPos;
	bool isDirty = true;
};

