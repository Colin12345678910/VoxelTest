#include "LitVoxelShader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
	return input.Color;
}