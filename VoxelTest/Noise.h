#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;
class Noise
{
public:
	float Perlin2D(float x, float y, float z);
	void Shuffle(int* arr);
	void MakePermutations();
	void Initialize();
	float Fade(float t);
private:
	static const int NUM_PERMUTATIONS = 256;
	int permutations[NUM_PERMUTATIONS * 2];

	static const int p[512];
	float Lerp(float x, float y, float t);
	Vector2 GetConstantVector(int v);
};

