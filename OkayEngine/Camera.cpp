#include "Camera.h"
#include "DX11.h"
#include "Engine.h"

Okay::Camera::Camera()
    :rot()
{
    using namespace DirectX;

    pos = XMVectorSet(10.f, 10.f, -10.f, 0);
    fwd = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    right = XMVectorSet(1.f, 0.f, 0.f, 0.f);

#ifdef EDITOR
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookToLH(pos, fwd, XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetMainAspectRatio(), 0.1f, 500.f)));
#else
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(pos, fwd, XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
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
    using namespace Okay;

    const float dt = Engine::GetDT();
    const float speed = 5.f * dt, rotSpeed = 3.f * dt;

    if (Engine::GetKeyDown(Keys::Left))
        rot.y -= rotSpeed;
    else if (Engine::GetKeyDown(Keys::Right))
        rot.y += rotSpeed;
    
    if (Engine::GetKeyDown(Keys::Up))
        rot.x -= rotSpeed;
    else if (Engine::GetKeyDown(Keys::Down))
        rot.x += rotSpeed;

    fwd = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMQuaternionRotationRollPitchYaw(rot.x, rot.y, 0.f));

    /*if (Engine::GetKeyDown(Keys::A))
        pos = XMVectorAdd(pos, fwd * speed);
    else if (Engine::GetKeyDown(Keys::D))
        rot.y += rotSpeed;*/

    if (Engine::GetKeyDown(Keys::W))
        pos = XMVectorAdd(pos, fwd * speed);
    else if (Engine::GetKeyDown(Keys::S))
        pos = XMVectorAdd(pos, fwd * -speed);


#ifdef EDITOR
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookToLH(pos, fwd, XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetMainAspectRatio(), 0.1f, 500.f)));
#else
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookAtLH(pos, fwd, XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetWindowAspectRatio(), 0.1f, 500.f)));
#endif

}
