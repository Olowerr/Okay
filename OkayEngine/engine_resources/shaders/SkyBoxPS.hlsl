#include "ShaderInclude.hlsli"

struct Input
{
	float4 pos : SV_POSITION;
	float3 localPos : LOCAL_POS;
};

float normalizeInThreshold(float value, float min, float max)
{
	return (value - min) / (max - min);
}

float4 main(Input input) : SV_TARGET
{
	const float4 skyTextureColour = skyBoxTexture.Sample(simp, input.localPos);
	const float3 normal = normalize(input.localPos);

	const float2 span = float2(1.f, 0.5f);

	float dotty = max(dot(sunDir, normal), 0.f);
	dotty = clamp(dotty, span.y, span.x);
	dotty = pow(dotty, sunSize);
	//dotty = normalizeInThreshold(dotty, span.y, span.x);

	return skyTextureColour + dotty * float4(sunColour, 1.f) * sunIntensity;
}