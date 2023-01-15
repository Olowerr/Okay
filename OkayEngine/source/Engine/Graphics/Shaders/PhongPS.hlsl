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

static float3 SUN_DIR = normalize(float3(1.f, -1.f, 1.f));

float4 main(TransformedVertex input) : SV_TARGET
{
#if 0
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

	return baseColour.Sample(simp, input.uv * uvTiling + uvOffset) * float4(shading.rgb, 1.f);

#elif 0
	return float4(baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb, 1.f);

#elif 0
	float3 colour = input.worldPos.y <= 250.f ? float3(0.f, 0.f, 1.f) : float3(0.f, 1.f, 0.f);
	return float4(colour, 1.f);


#elif 0
	float3 colour = float3(1.f, 0.f, 0.f);// baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb;
	return float4(max(dot(-input.normal, SUN_DIR), 0.f) * colour, 1.f);


#elif 1

	float3 colour = baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb;
	return float4(max(dot(-input.normal, SUN_DIR), 0.f) * colour + colour * 0.2f, 1.f);

#else
	return float4(input.normal, 1.f);
#endif
}