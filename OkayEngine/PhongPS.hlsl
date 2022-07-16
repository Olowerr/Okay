#include "ShaderInclude.hlsli"

float4 main(TransformedVertex input) : SV_TARGET
{
	return float4(input.normal, 1.f);
	float q = (input.svPos.z / input.svPos.w);
	q = pow(q, 0.5f);
	return float4(q,q,q,q) * 1.f;
}