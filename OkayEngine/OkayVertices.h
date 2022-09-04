#pragma once
#include <DirectXMath.h>

namespace Okay
{
	using Float4 = DirectX::XMFLOAT4;
	using Float3 = DirectX::XMFLOAT3;
	using Float2 = DirectX::XMFLOAT2;

	struct UVNormal
	{
		UVNormal() = default;
		~UVNormal() = default;

		Float2 uv;
		Float3 normal;
	};

	struct SkinnedVertex
	{
		unsigned int jointIdx[4]{};
		float weight[4]{};
	};
}