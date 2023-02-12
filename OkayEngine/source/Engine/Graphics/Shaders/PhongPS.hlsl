#include "ShaderInclude.hlsli"

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D ambientTexture: register(t2);

cbuffer MatData : register(b3)
{
	float2 uvTiling;
	float2 uvOffset;
	float shinyness;

	float matPadding1;
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

float3 OkayReflect(float3 v, float3 n)
{
	return 2 * dot(v, n) * n - v;
}

float4 main(TransformedVertex input) : SV_TARGET
{
#if 1
	const float2 uv = input.uv * uvTiling + uvOffset;
	const float4 baseColour = diffuseTexture.Sample(simp, uv);
	const float4 specular = specularTexture.Sample(simp, uv);
	const float4 ambient = ambientTexture.Sample(simp, uv);
	const float3 posToCam = normalize(camPos - input.worldPos);

	PointLight poLight;
	DirectionalLight dirLight;

	float distance;
	float3 posToLight;

	float4 diffShading = float4(0.f, 0.f, 0.f, 1.f);
	float4 specShading = float4(0.f, 0.f, 0.f, 1.f);

	uint i = 0;

	// TODO: Remove: (dot(posToLight, input.normal) > 0.f).
	// Without it points lit up even when the light source was inside the object.
	// But to my understanding, it should work without the extra check

	// Point Lights 
	for (i = 0; i < numPoint; i++)
	{
		poLight = pointLights[i];

		posToLight = poLight.position - input.worldPos;
		distance = length(posToLight);
		posToLight /= distance;

		const float attu = (1.f / (1.f + poLight.attenuation.x * distance + poLight.attenuation.y * distance * distance));

		specShading += specular * pow(max(dot(reflect(-posToLight, input.normal) * (dot(posToLight, input.normal) > 0.f), posToCam), 0.f), shinyness) *
						float4(poLight.colour, 1.f) * poLight.intensity * attu;

		diffShading += baseColour * max(dot(input.normal, posToLight), 0.f) * float4(poLight.colour, 1.f) * poLight.intensity * attu;
	}

	// Directional Lights
	for (i = 0; i < numDir; i++)
	{
		dirLight = dirLights[i];

		specShading += specular * pow(max(dot(reflect(dirLight.direction, input.normal) * (dot(-dirLight.direction, input.normal) > 0.f), posToCam), 0.f), shinyness) *
			float4(dirLight.colour, 1.f) * dirLight.intensity;

		diffShading += max(dot(input.normal, -dirLight.direction), 0.f) * float4(dirLight.colour, 1.f) * dirLight.intensity;
	}

	
	return saturate(baseColour * (baseColour * 0.2f + diffShading + specShading));

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