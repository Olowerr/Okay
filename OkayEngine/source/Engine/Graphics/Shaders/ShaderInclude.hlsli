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

cbuffer Transform : register(b1)
{
	float4x4 worldMatrix;
};