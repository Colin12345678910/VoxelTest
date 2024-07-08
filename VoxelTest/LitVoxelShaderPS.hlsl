#include "LitVoxelShader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{



	////float3 SPOS = mul(shadowCoord, input.WorldPos);
	//float4 lightPos = mul(shadowScaler, input.LightPos);
	
	//float3 shadowPos = input.LightPos.xyz / input.LightPos.w; //Apparently the W in this case is 0, and causes a failure.
	//float currentDepth = shadowPos.z;

	//shadowPos = shadowPos * 0.5 + 0.5;

	//shadowPos.y = 1 - shadowPos.y;

	//float shadowSample = shadowTex.Sample(sam, shadowPos.xy).x;
	
	//if(currentDepth - 0.00015f > shadowSample)
	//{
	//return float4(input.Color.xyz * 0.3, 1.0f);
	//}
	
	float shadowSample = ShadowSample(input.LightPos);
	
	return float4(input.Color.xyz - shadowSample, 1.0f);
}