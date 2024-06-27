#include "LitVoxelShader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{



	////float3 SPOS = mul(shadowCoord, input.WorldPos);
	//float4 lightPos = mul(shadowScaler, input.LightPos);
	
	float3 shadowPos = input.LightPos.xyz / input.LightPos.w; //Apparently the W in this case is 0, and causes a failure.
	float currentDepth = shadowPos.z;

	shadowPos = shadowPos * 0.5 + 0.5;
	
	//shadowPos = shadowPos
	//return float4(input.LightPos.xyz, 1.0f);

	shadowPos.y = 1 - shadowPos.y;

	float shadowSample = shadowTex.Sample(sam, shadowPos.xy).x;
	
	//float currentDepth = shadowPos.z;

	//return float4(shadowSample.xxx, 1.0f);

	if(currentDepth > shadowSample)
	{
	return float4(0.8f, 0.8f, 0.8f, 1.0f);
	}
	
	
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
	////return float4(input.Color);
}