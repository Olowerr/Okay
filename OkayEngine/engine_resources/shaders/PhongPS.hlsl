#include "ShaderInclude.hlsli"

Texture2D baseColour : register(t0);
Texture2D specular : register(t1);
Texture2D ambient : register(t2);

cbuffer MatData : register(b3)
{
	float2 uvTiling;
	float2 uvOffset;
}

struct PointLight
{
	float3 colour;
	float intensity;
	float2 attenuation;
	float3 position;
};

StructuredBuffer<PointLight> pointLights : register(t3);

cbuffer lightInfo : register(b4)
{
	uint numPoint;
}

SamplerState simp : register(s0);

float4 main(TransformedVertex input) : SV_TARGET
{
#if 1
	PointLight poLight;

	float distance;
	float finalIntensity;
	float3 lightVec;

	float3 shading = float3(0.f, 0.f, 0.f);

	// Point Lights // temp calculations
	for (uint i = 0; i < numPoint; i++)
	{
		poLight = pointLights[i];

		lightVec = poLight.position - input.worldPos;
		distance = length(lightVec);
		lightVec /= distance;

		finalIntensity = (1.f / (1.f + poLight.attenuation.x * distance + poLight.attenuation.y * distance * distance));
		finalIntensity *= max(dot(input.normal, lightVec), 0.f);
		finalIntensity *= poLight.intensity;

		shading += finalIntensity * poLight.colour;
	}

	//specular.Sample(simp, input.uv), ambient.Sample(simp, input.uv);
	return baseColour.Sample(simp, input.uv * uvTiling + uvOffset) * float4(shading.rgb, 1.f);
#else
	return float4(baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb, 0.f);
#endif
}