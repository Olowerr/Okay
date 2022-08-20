struct InputVertex
{
	float3 pos : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	uint4 jointIdx : JOINTIDX;
	float4 weight : WEIGHTS;
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

struct Joint
{
	float4x4 transform;
};

StructuredBuffer<Joint> joints : register(t0);

TransformedVertex main(InputVertex input)
{
	TransformedVertex output;

	output.worldPos = mul(float4(input.pos, 1.f), joints[input.jointIdx.x].transform).xyz;

	output.svPos = mul(float4(output.worldPos, 1.f), viewProjectMatrix);

	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}