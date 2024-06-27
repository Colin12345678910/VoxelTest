#include "pch.h"
#include "Chunk.h"

void Chunk::Release()
{
	isDirty = true;
	delete blocks;
	delete isEmpty;
}

Chunk::Chunk()
{
	blocks = new USHORT[NUM_BLOCKS];
	isEmpty = new bool[height];
	for (int i = 0; i < NUM_BLOCKS; i++) {
		blocks[i] = 0;
	}
	for (int i = 0; i < Chunk::height; i++)
	{
		isEmpty[i] = true;
	}
}

Chunk::~Chunk()
{
	
}

void Chunk::FillBlocks(Noise &noise, Vector2 chunk)
{
	//for (int i = 0; i < NUM_BLOCKS; i++) {
	//	blocks[i] = 0;
	//}
	//for (int i = 0; i < 64; i++)
	//{
	//	blocks[i] = 1;
	//}
	worldXPos = chunk.x;
	worldYPos = chunk.y;
	chunk *= width;
	
	
	for (int x = 0; x < Chunk::width; x++) {
		for (int z = 0; z < Chunk::depth; z++) {
			float no = 16 * noise.Perlin2D((x + chunk.x) * 0.052f, (z + chunk.y) * 0.052f, z * 0.052f); //-1 to 1
			no *= 8 * noise.Perlin2D((x + chunk.x) * 0.02f, (z + chunk.y) * 0.02f, z * 0.02f); //-1 to 1
			no *= 2 * noise.Perlin2D((x + chunk.x) * 0.06f, (z + chunk.y) * 0.06f, z * 0.06f); //-1 to 1
			no *= 1 * noise.Perlin2D((x + chunk.x) * 0.12f, (z + chunk.y) * 0.12f, z * 0.12f); //-1 to 1

			float upper = 32 * noise.Perlin2D((x + chunk.x) * 0.02f, (z + chunk.y) * 0.02f, z * 0.02f);
			for (int y = 0; y < Chunk::height; y++) {
				int i = ConvertPosToIndex(Vector3(x,y,z));
				
				
				float noi = 8 * no + 64;

				if (worldXPos == 0 && worldYPos > 0)
				{

					//if (y > noi && y < (64 - upper)) {
					if (y < upper + 128) {
						blocks[i] = 1;
						isEmpty[y] = false;
					}
					else {
						blocks[i] = 0;
					}
				}
				if (y == 1)
				{
					blocks[i] = 1;
				}
			}
		}
	}
	
	isDirty = false;
}

int Chunk::GetBlock(Vector3 pos)
{
	int blockLocation = ConvertPosToIndex(pos);

	if (pos.y >= height)
		return 0;
	if (pos.y < 0) {
		return 0;
	}
	if (pos.x >= width)
		return 0;
	if (pos.x < 0) {
		return 0;
	}
	if (pos.z >= depth)
		return 0;
	if (pos.z < 0) {
		return 0;
	}
	return blocks[blockLocation];
}

int Chunk::GetBlock(Vector3 pos, bool& up, bool& down, bool& left, bool& right, bool& front, bool& back)
{
	up = down = left = right = front = back = true;
	int blockLocation = ConvertPosToIndex(pos);
	
	if (GetBlock(pos + Vector3::Up) != 0) {
		up = false;
	}
	if (GetBlock(pos + Vector3::Down) != 0) {
		down = false;
	}
	if (GetBlock(pos + Vector3::Left) != 0) {
		left = false;
	}
	if (GetBlock(pos + Vector3::Right) != 0) {
		right = false;
	}
	if (GetBlock(pos + Vector3::Forward) != 0) {
		front = false;
	}
	if (GetBlock(pos + Vector3::Backward) != 0) {
		back = false;
	}

	return blocks[blockLocation];
}

void Chunk::SetBlock(int x, int y, int z, int block)
{
	blocks[ConvertPosToIndex(Vector3(x, y, z))] = block;
	isEmpty[y] = false;
}

int Chunk::ConvertPosToIndex(Vector3 pos)
{
	//return (pos.z * width * height) + (pos.y * width) + pos.x;
	return width * ((pos.z * height) + (pos.y)) + pos.x;
}
