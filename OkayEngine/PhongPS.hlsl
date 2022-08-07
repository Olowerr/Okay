#include "ShaderInclude.hlsli"

cbuffer MatData : register(b3)
{
	float2 uvTiling;
	float2 uvOffset;
}

cbuffer PointLight :  register(b4)
{
	float intensity;
	float3 colour;
	float2 attenuation;
}

SamplerState simp : register(s0);

float4 main(TransformedVertex input) : SV_TARGET
{
	float length = 

	const finalIntensity = intensity * (1.f / (1.f + attenuation.x))
	float4 colour = baseColour.Sample(simp, input.uv * uvTiling + uvOffset);


	return baseColour.Sample(simp, input.uv * uvTiling + uvOffset);// *specular.Sample(simp, input.uv)* ambient.Sample(simp, input.uv);
}