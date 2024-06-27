
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