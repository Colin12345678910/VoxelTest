
cbuffer Constants : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix worldViewProjectionMatrix;
    matrix shadowCoord;
};

Texture2D shadowTex : register(t0);
sampler sam : register(s0);



struct VS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 LightPos : TEXCOORD0;
    float3 WorldPosition : TEXCOORD1;
    float4 Color : COLOR;
    
};

float ShadowSample(float4 lightPos)
{
    float3 shadowPos = lightPos.xyz / lightPos.w; //Apparently the W in this case is 0, and causes a failure.
    float currentDepth = shadowPos.z;

    shadowPos = shadowPos * 0.5 + 0.5;

    shadowPos.y = 1 - shadowPos.y;
    float shadowSample = 0;
    float shadowAmount;
    
    for (int x = -1; x < 2; x++)
    {
        for (int y = -1; y < 2; y++)
        {
            //4096 is picked because of shadowmap resolution, this ideally should be in shaderconsts, however for the purposes of demonstration, I simply used the same number here as I did when making the texture.
            // The 0.5 is here to just tidy the texture sampling a little bit, as I wanted sharper edges.
            //shadowSample = shadowTex.Sample(sam, shadowPos.xy + float2(x, y) * 1/4096 * 0.5).x; These two lines are equivelant, but the second one is mathmetically simplified.
            shadowSample = shadowTex.Sample(sam, shadowPos.xy + float2(x, y) * 0.00012207031).x; // 1/4096 = 0.00024414062 0.00024414062 * 0.5 = 0.00012207031
            
            if (currentDepth - 0.00015f > shadowSample)
            {
                shadowAmount++;
            }
        }

    }
    
    return shadowAmount / 16 + 7/16;
    

    //float shadowSample = shadowTex.Sample(sam, shadowPos.xy).x;
	
    //if (currentDepth - 0.00015f > shadowSample)
    //{
    //    return 0.3f;
    //}
    //else
    //{
    //    return 1.0f;
    //}
}