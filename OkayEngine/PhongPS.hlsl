#include "ShaderInclude.hlsli"

cbuffer MatData : register(b3)
{
	float2 uvTiling;
	float2 uvOffset;
}

SamplerState simp : register(s0);

float4 main(TransformedVertex input) : SV_TARGET
{
	return baseColour.Sample(simp, input.uv * uvTiling + uvOffset);// *specular.Sample(simp, input.uv)* ambient.Sample(simp, input.uv);

	return float4(input.normal, 1.f);

	float q = (input.svPos.z / input.svPos.w);
	q = pow(q, 0.5f);
	return float4(q,q,q,q) * 1.f;
}