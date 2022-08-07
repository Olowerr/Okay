struct InputVertex
{
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

struct TransformedVertex
{
	float4 svPos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;

	float3 worldPos : W_POS;
};

cbuffer Camera : register(b0)
{
	//float4x4 viewMatrix;
	//float4x4 projectMatrix;
	float4x4 viewProjectMatrix;

	/*float3 camPos;
	float camPad0;

	float3 camDir;
	float camPad1;*/
};

cbuffer Transform : register(b1) // will change with Instancing
{
	float4x4 worldMatrix;
};

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
	float3 position;
	float intensity;
	float3 colour;
	float2 attenuation;
};

StructuredBuffer<PointLight> pointLights : register(t3);

cbuffer lightInfo : register(b4)
{
	uint numPoint;
}

SamplerState simp : register(s0);

float4 main(TransformedVertex input) : SV_TARGET
{
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
}