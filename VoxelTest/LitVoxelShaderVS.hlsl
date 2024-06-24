#include "LitVoxelShader.hlsli"

PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;
	output.Pos = mul(input.Pos, worldViewProjectionMatrix);
	output.Color = input.Color;
	return output;
}