#include "ShaderInclude.hlsli"

TransformedVertex main( InputVertex input)
{
	TransformedVertex output;
	output.worldPos = float3(1, 1, 1);
	output.normal = float3(2, 2, 2);

	//output.worldPos = mul(float4(input.pos, 1.f), worldMatrix).xyz;
	output.svPos = mul(float4(input.pos, 1.f), viewProjectMatrix);


	output.normal = mul(float4(input.normal, 0.f), worldMatrix).xyz;
	output.uv = input.uv;

	return output;
}