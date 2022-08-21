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


#if 1
	output.worldPos = float3(0.f, 0.f, 0.f);
	output.worldPos += mul(float4(input.pos, 1.f), joints[input.jointIdx.x].transform).xyz * input.weight.x;
	output.worldPos += mul(float4(input.pos, 1.f), joints[input.jointIdx.y].transform).xyz * input.weight.y;
	output.worldPos += mul(float4(input.pos, 1.f), joints[input.jointIdx.z].transform).xyz * input.weight.z;
	output.worldPos += mul(float4(input.pos, 1.f), joints[input.jointIdx.w].transform).xyz * input.weight.w;
#else
	float4x4 mat = joints[input.jointIdx.x].transform * input.weight.x;
	mat += joints[input.jointIdx.y].transform * input.weight.y;
	mat += joints[input.jointIdx.z].transform * input.weight.z;
	mat += joints[input.jointIdx.w].transform * input.weight.w;
	output.worldPos = mul(float4(input.pos, 1.f), mat).xyz;
#endif	

	output.svPos = mul(float4(output.worldPos, 1.f), viewProjectMatrix);

	output.normal = mul(float4(input.normal, 0.f), joints[input.jointIdx.x].transform).xyz;
	output.uv = input.uv;

	return output;
}