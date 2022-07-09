#pragma once
#include "DX11.h"
#include "OkayVertices.h"

namespace Okay
{
	class Camera
	{
	public:
		Camera();
		~Camera();
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;

		virtual void SetPosition(Float3 position);
		const DirectX::XMFLOAT4X4& GetViewProjectMatrix() const;

		virtual void Update();

	protected:
		DirectX::XMFLOAT4X4 viewProject;

		// TODO: Implement Camera movement

	};
}