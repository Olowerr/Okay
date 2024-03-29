#include "ShaderInclude.hlsli"


TransformedVertex main(InputVertex input)
{
	TransformedVertex output;

	if (heightMapBool)
	{
		input.pos.y += heightMap.SampleLevel(simp, input.uv, 0.f).r * heightMapScalar;
	}

	output.worldPos = mul(float4(input.pos, 1.f), worldMatrix).xyz;
	output.svPos = mul(float4(output.worldPos, 1.f), viewProjectMatrix);

	output.normal = normalize(mul(float4(input.normal, 0.f), worldMatrix).xyz);
	output.uv = input.uv;

	return output;
}