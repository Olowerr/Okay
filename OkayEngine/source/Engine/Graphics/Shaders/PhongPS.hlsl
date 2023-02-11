#include "ShaderInclude.hlsli"

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D ambientTexture: register(t2);

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

struct DirectionalLight
{
	float3 colour;
	float intensity;
	float3 direction;
};

StructuredBuffer<PointLight> pointLights : register(t3);
StructuredBuffer<DirectionalLight> dirLights : register(t4);

cbuffer lightInfo : register(b4)
{
	uint numPoint;
	uint numDir;
}

SamplerState simp : register(s0);

static float3 SUN_DIR = normalize(float3(1.f, -1.f, 1.f));

float4 main(TransformedVertex input) : SV_TARGET
{
#if 1
	const float2 uv = input.uv * uvTiling + uvOffset;
	const float4 baseColour = diffuseTexture.Sample(simp, uv);
	const float4 specular = specularTexture.Sample(simp, uv);

	PointLight poLight;
	DirectionalLight dirLight;

	float distance;
	float4 finalIntensity;
	float3 lightVec;

	float4 shading = float4(0.f, 0.f, 0.f, 1.f);

	uint i = 0;

	// temp calculations
	// Point Lights 
	for (i = 0; i < numPoint; i++)
	{
		poLight = pointLights[i];

		lightVec = poLight.position - input.worldPos;
		distance = length(lightVec);
		lightVec /= distance;

		float3 toCam = normalize(camPos - input.worldPos);

		const float attu = (1.f / (1.f + poLight.attenuation.x * distance + poLight.attenuation.y * distance * distance));

		finalIntensity = baseColour * max(dot(input.normal, lightVec), 0.f) + specular * max(dot(camDir, reflect(toCam, input.normal)), 0.f);
		finalIntensity *= poLight.intensity * attu;

		shading += finalIntensity * float4(poLight.colour, 1.f);
	}

	for (i = 0; i < numDir; i++)
	{
		dirLight = dirLights[i];

		finalIntensity = max(dot(input.normal, -dirLight.direction), 0.f);
		
		shading += finalIntensity * float4(dirLight.colour, 1.f);
	}

	
	return baseColour * shading;

#elif 0
	return float4(baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb, 1.f);

#elif 0
	float3 colour = input.worldPos.y <= 250.f ? float3(0.f, 0.f, 1.f) : float3(0.f, 1.f, 0.f);
	return float4(colour, 1.f);


#elif 0
	float3 colour = float3(1.f, 0.f, 0.f);// baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb;
	return float4(max(dot(-input.normal, SUN_DIR), 0.f) * colour, 1.f);


#elif 0

	float3 colour = baseColour.Sample(simp, (input.uv + uvOffset) * uvTiling).rgb;
	return float4(max(dot(-input.normal, SUN_DIR), 0.2f) * colour, 1.f);

#else
	return float4(input.normal, 1.f);
#endif
}