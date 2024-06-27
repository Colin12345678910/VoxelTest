#include "LitVoxelShader.hlsli"
matrix shadowScaler =
{
    0.5f, 0.0f, 0.0f, 0.5f,
    0.0f, 0.5f, 0.0f, 0.5f,
    0.0f, 0.0f, 0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 1.0f
    
};
PS_INPUT main( VS_INPUT input )
{
	PS_INPUT output;
	output.Pos = mul(input.Pos, worldViewProjectionMatrix);
	output.WorldPosition = mul(input.Pos, worldMatrix).xyz;
	output.LightPos = mul(input.Pos, shadowCoord);
	output.Color = input.Color;


	
	return output;
}