#pragma once
#include "Okay/Okay.h"

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

		const DirectX::XMFLOAT4X4& GetViewProjectMatrix() const;

		virtual void Update();

	protected:
		DirectX::XMFLOAT4X4 viewProject;

		Float2 rot;

		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR fwd, right;


	};
}