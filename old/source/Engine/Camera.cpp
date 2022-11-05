#include "Camera.h"
#include "DX11.h"
#include "Engine.h"
#include "Application/Keys.h"

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

    const float Dt = Engine::GetDT();
    float speed = 5.f * Dt, rotSpeed = 3.f * Dt;

    if (Engine::GetKeyDown(Keys::Shift))
    {
        speed *= 0.25f;
        rotSpeed *= 0.5f;
    }

    if (Engine::GetKeyDown(Keys::Left))
        rot.y -= rotSpeed;
    else if (Engine::GetKeyDown(Keys::Right))
        rot.y += rotSpeed;
    
    if (Engine::GetKeyDown(Keys::Up))
        rot.x -= rotSpeed;
    else if (Engine::GetKeyDown(Keys::Down))
        rot.x += rotSpeed;
    
    if (rot.x > XM_PIDIV2)
        rot.x = XM_PIDIV2;
    else if (rot.x < -XM_PIDIV2)
        rot.x = -XM_PIDIV2;

    //printf("X: %f\nY: %f\n\n", rot.x, rot.y);

    static XMVECTOR vector;
    vector = XMQuaternionRotationRollPitchYaw(rot.x, rot.y, 0.f);
    fwd = XMVector3Rotate(Okay::FORWARD, vector);
    right = XMVector3Rotate(Okay::RIGHT, vector);

    if (Engine::GetKeyDown(Keys::A))
        pos = XMVectorAdd(pos, right * -speed);
    else if (Engine::GetKeyDown(Keys::D))
        pos = XMVectorAdd(pos, right * speed);
    
    if (Engine::GetKeyDown(Keys::W))
        pos = XMVectorAdd(pos, fwd * speed);
    else if (Engine::GetKeyDown(Keys::S))
        pos = XMVectorAdd(pos, fwd * -speed);

    if (Engine::GetKeyDown(Keys::Space))
        pos.m128_f32[1] += speed;
    else if (Engine::GetKeyDown(Keys::Control))
        pos.m128_f32[1] -= speed;



    vector = XMVector3Cross(fwd, right);
#ifdef EDITOR
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookToLH(pos, fwd, vector) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetMainAspectRatio(), 0.1f, 500.f)));
#else
    XMStoreFloat4x4(&viewProject, XMMatrixTranspose(
        XMMatrixLookToLH(pos, fwd, vector) *
        XMMatrixPerspectiveFovLH(XM_PIDIV2, DX11::Get().GetWindowAspectRatio(), 0.1f, 500.f)));
#endif

}
