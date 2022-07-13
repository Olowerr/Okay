#include "ShaderInclude.hlsli"

float4 main(TransformedVertex input) : SV_TARGET
{
	float q = (input.svPos.z / input.svPos.w);
	q = pow(q, 0.5f);
	return float4(q,q,q,q) * 1.f;
}