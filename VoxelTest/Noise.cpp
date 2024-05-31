#include "pch.h"
#include "Noise.h"
#include "SimpleMath.h"
//Ref https://rtouti.github.io/graphics/perlin-noise-algorithm

const int Noise::p[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

float Noise::Perlin2D(float x, float y, float z)
{
	int xFloor = (int)floorf(x) & 255;
	int yFloor = (int)floorf(y) & 255;

	//if (yFloor >= 1) {
	//	return 0;
	//}
	
	float xf = x - floorf(x);
	float yf = y - floorf(y);

	//Vector2 topRight = Vector2(xf - 1.0, yf - 1.0);
	//Vector2 topLeft = Vector2(xf, yf - 1.0);
	//Vector2 bottomRight = Vector2(xf - 1.0, yf);
	//Vector2 bottomLeft = Vector2(xf, yf);

	Vector2 topRight = Vector2(xf - 1.0, yf - 1.0);
	Vector2 topLeft = Vector2(xf, yf - 1.0);
	Vector2 bottomRight = Vector2(xf - 1.0, yf);
	Vector2 bottomLeft = Vector2(xf, yf);

	float valTopRight = permutations[permutations[xFloor + 1] + yFloor + 1];
	float valTopLeft = permutations[permutations[xFloor] + yFloor + 1];
	float valBottomRight = permutations[permutations[xFloor + 1] + yFloor];
	float valBottomLeft = permutations[permutations[xFloor] + yFloor];

	//float valTopLeft = p[p[xFloor] + yFloor + 1];
	//float valTopRight = p[p[xFloor + 1] + yFloor + 1];
	//
	//float valBottomLeft = p[p[xFloor] + yFloor];
	//float valBottomRight = p[p[xFloor + 1] + yFloor];
	

	float dotTopRight = topRight.Dot(GetConstantVector(valTopRight));
	float dotTopLeft = topLeft.Dot(GetConstantVector(valTopLeft));
	float dotBottomRight = bottomRight.Dot(GetConstantVector(valBottomRight));
	float dotBottomLeft = bottomLeft.Dot(GetConstantVector(valBottomLeft));

	float u = Fade(xf);
	float v = Fade(yf);

	float result = Lerp(u,
		Lerp(v, dotBottomLeft, dotTopLeft),
		Lerp(v, dotBottomRight, dotTopRight)
		
		
	);


	
	return result;
}

void Noise::Shuffle(int* arr)
{
	srand(time(0));
	for(int i = NUM_PERMUTATIONS - 1; i > 0; i--)
	{
		int index = rand() % (i);
		int temp = arr[index];

		arr[i] = temp;
		arr[index] = temp;

	}
}

void Noise::MakePermutations()
{
	for (int i = 0; i < NUM_PERMUTATIONS; i++) {
		permutations[i] = i;
	}

	Shuffle(permutations);

	for (int i = NUM_PERMUTATIONS; i < NUM_PERMUTATIONS * 2; i++) 
	{
		permutations[i] = permutations[i - NUM_PERMUTATIONS];
	}

	//TODO :: Shuffle Permutations
}

void Noise::Initialize()
{
	MakePermutations();
}

float Noise::Fade(float t) 
{
	return t * t * t * (t * (t * 6 - 15) + 10);
	//return t;
}

float Noise::Lerp(float t, float x, float y)
{
	return x + t * (y - x);
}

Vector2 Noise::GetConstantVector(int v)
{
	int h = v & 3; //Equivelant to %4, might be more performant? Used by guide.
	
	if (h == 0) {
		return Vector2(1.0, 1.0);
	}
	else if (h == 1) {
		return Vector2(-1.0, 1.0);
	}
	else if (h == 2) {
		return Vector2(-1.0, -1.0);
	}
	return Vector2(1.0, -1.0);
}
