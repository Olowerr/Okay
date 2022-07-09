#include "Camera.h"

Okay::Camera::Camera()
{
    using namespace DirectX;
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, -3.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, WinRatio, 0.1f, 500.f)));
}

Okay::Camera::~Camera()
{
}

const DirectX::XMFLOAT4X4& Okay::Camera::GetViewProjectMatrix() const
{
    return viewProject;
}

void Okay::Camera::Update()
{

}
