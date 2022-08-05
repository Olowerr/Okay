#include "Camera.h"
#include "DX11.h"
#include "Engine.h"

Okay::Camera::Camera()
{
    using namespace DirectX;

    pos = { 10.f, 10.f, -10.f };

#ifdef EDITOR
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(XMVectorSet(pos.x, pos.y, pos.z, 0.f), XMVectorSet(0.f, 5.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetMainAspectRatio(), 0.1f, 500.f)));
#else
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(XMVectorSet(pos.x, pos.y, pos.z, 0.f), XMVectorSet(0.f, 5.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetWindowAspectRatio(), 0.1f, 500.f)));
#endif

    
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
    using namespace DirectX;

    static float speed = 5.f;

    const float dt = Engine::GetDT();

    if (Okay::Engine::GetKeyDown(Keys::W))
        pos.z += speed * dt;

    else if (Okay::Engine::GetKeyDown(Keys::S))
        pos.z -= speed * dt;

    else if (Okay::Engine::GetKeyDown(Keys::R))
        pos = { 10.f, 10.f, -10.f };

#ifdef EDITOR
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(XMVectorSet(pos.x, pos.y, pos.z, 0.f), XMVectorSet(0.f, 5.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetMainAspectRatio(), 0.1f, 500.f)));
#else
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(XMVectorSet(pos.x, pos.y, pos.z, 0.f), XMVectorSet(0.f, 5.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetWindowAspectRatio(), 0.1f, 500.f)));
#endif

}
