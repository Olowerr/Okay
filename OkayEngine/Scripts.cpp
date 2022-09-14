#include "Scripts.h"
#include "Engine.h"

#include "imgui/imgui.h"

void ScriptRotate::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.rotation.y += 2.f * Okay::Engine::GetDT();
	tra.CalcMatrix();
}

void ScriptHover::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.position.y = initPosY + 1.5f * std::sin(Okay::Engine::GetUpTime());
	tra.CalcMatrix();
}

void ScriptCameraMovement::Start()
{
	using namespace DirectX;

	XMVECTOR vector = XMQuaternionRotationRollPitchYaw(camRot.x, camRot.y, 0.f);
	fwd = XMVector3Rotate(Okay::FORWARD, vector);
	up = XMVector3Rotate(Okay::UP, vector);
	pos = XMVectorAdd(XMLoadFloat3(&tra.position), XMVectorScale(fwd, -5.f));

	cam.Update(pos, XMLoadFloat3(&tra.position), up);
}

void ScriptCameraMovement::Update()
{
	static float maxXRot = DirectX::XM_PIDIV4;
	static float minXRot = -DirectX::XM_PIDIV4;
	static float camDist = 20.f;
	static float camHeight = 12.f;

	if (ImGui::Begin("Yas"))
	{
		ImGui::PushItemWidth(-1.f);
		
		ImGui::Text("CamDistance"); ImGui::SameLine();
		ImGui::DragFloat("##CamDistance", &camDist, 0.01f);

		ImGui::Text("CamHeight  "); ImGui::SameLine();
		ImGui::DragFloat("##CamHeight", &camHeight, 0.01f);
		
		ImGui::Text("Sens       "); ImGui::SameLine();
		ImGui::DragFloat("##Sens", &sens, 0.01f, 0.f, 20.f);
		
		ImGui::Text("MaxRotation"); ImGui::SameLine();
		ImGui::DragFloat("##MaxRotation", &maxXRot, 0.001f);
		
		ImGui::Text("MinRotation"); ImGui::SameLine();
		ImGui::DragFloat("##MinRotation", &minXRot, 0.001f);

		ImGui::PopItemWidth();
	}
	ImGui::End();

	using namespace DirectX;
	if (Okay::Engine::GetMouseLocked())
	{
		const float XInput = Okay::Engine::GetMouseDeltaX();
		const float YInput = Okay::Engine::GetMouseDeltaY();

		camRot.x += sens * YInput * Okay::Engine::GetDT();
		camRot.y += sens * XInput * Okay::Engine::GetDT();
	}

	if (camRot.x >= maxXRot)
		camRot.x = maxXRot;
	else if (camRot.x <= minXRot)
		camRot.x = minXRot;

	XMVECTOR targetPos = XMLoadFloat3(&tra.position);
	targetPos.m128_f32[1] += camHeight;

	XMVECTOR vector = XMQuaternionNormalize(XMQuaternionRotationRollPitchYaw(camRot.x, camRot.y, 0.f));
	fwd = XMVector3Normalize(XMVector3Rotate(Okay::FORWARD, vector));
	up =  XMVector3Normalize(XMVector3Rotate(Okay::UP, vector));
	pos = XMVectorAdd(targetPos, XMVectorScale(fwd, -camDist));

	printf("Tar: %f, %f, %f\nCam: %f, %f, %f\n\n",
		tra.position.x, tra.position.y, tra.position.z,
		pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
	

	cam.Update(pos, targetPos, up);
}

void ScriptBasicMovement::Start()
{
	cam = &GetScript<ScriptCameraMovement>(); 
}

void ScriptBasicMovement::Update()
{
	static float speed = 20.f;

	if (ImGui::Begin("Yas"))
	{
		ImGui::PushItemWidth(-1.f);

		ImGui::Text("Speed      "); ImGui::SameLine();
		ImGui::DragFloat("##Speed", &speed, 0.01f, 0.f, 20.f);

		ImGui::PopItemWidth();
	}
	ImGui::End();

	using namespace DirectX;
	if (!Okay::Engine::GetMouseLocked())
		return;

	const float ZInput = Okay::Engine::GetKeyDown(Keys::W) ? 1.f : Okay::Engine::GetKeyDown(Keys::S) ? -1.f : 0.f;
	const float XInput = Okay::Engine::GetKeyDown(Keys::D) ? 1.f : Okay::Engine::GetKeyDown(Keys::A) ? -1.f : 0.f;
	const float frameSpeed = speed * Okay::Engine::GetDT();

	XMVECTOR yasFwd = cam->fwd;
	yasFwd.m128_f32[1] = 0.f;
	yasFwd = XMVectorScale(XMVector3Normalize(yasFwd), frameSpeed * ZInput);

	XMVECTOR yasSide = XMVectorScale(XMVector3Normalize(XMVector3Cross(cam->up, cam->fwd)), frameSpeed * XInput);

	XMStoreFloat3(&tra.position, XMVectorAdd(yasFwd, XMLoadFloat3(&tra.position)));
	XMStoreFloat3(&tra.position, XMVectorAdd(yasSide, XMLoadFloat3(&tra.position)));


	tra.CalcMatrix();
}