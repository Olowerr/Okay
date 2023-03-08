typedef int BOOL;

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

struct PointLight
{
	// Component
	float3 colour;
	float intensity;
	float2 attenuation;

	// Transform
	float3 position;
};

struct DirectionalLight
{
	// Component
	float3 colour;
	float intensity;

	// Transform
	float3 direction;
};

// Buffers
cbuffer Camera : register(b0)
{
	//float4x4 viewMatrix;
	//float4x4 projectMatrix;
	float4x4 viewProjectMatrix;

	float3 camPos;
	float camPad0;

	float3 camDir;
	float camPad1;
};

cbuffer Transform : register(b1)
{
	float4x4 worldMatrix;
};

cbuffer MatData : register(b2)
{
	float2 uvTiling;
	float2 uvOffset;
	float shinyness;

	float matPadding1;
}

cbuffer ShaderData : register(b3)
{
	BOOL heightMapBool;
	float heightMapScalar;
	float padding[2];
};

cbuffer SkyData : register(b4)
{
	float3 ambientTint;
	float ambientTintIntensity;

	float3 sunDir;
	float sunSize;

	float3 sunColour;
	float sunIntensity;
}

cbuffer lightInfo : register(b5)
{
	uint numPoint;
	uint numDir;
}

// Resources
Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D ambientTexture: register(t2);

Texture2D heightMap : register(t3);

StructuredBuffer<PointLight> pointLights : register(t4);
StructuredBuffer<DirectionalLight> dirLights : register(t5);

TextureCube skyBoxTexture : register(t6);

// Samplers
SamplerState simp : register(s0);