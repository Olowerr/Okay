#include "ShaderInclude.hlsli"

struct Input
{
	float4 pos : SV_POSITION;
	float3 localPos : LOCAL_POS;
};

float4 main(Input input) : SV_TARGET
{
	const float4 skyTextureColour = skyBoxTexture.Sample(simp, input.localPos);
	const float3 normal = normalize(input.localPos);

	float dotty = max(dot(sunDir, normal), 0.f);
	dotty = pow(dotty, sunSize);

	return skyTextureColour + dotty * float4(sunColour, 1.f) * sunIntensity;
}