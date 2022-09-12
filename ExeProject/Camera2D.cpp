#include "Camera2D.h"
#include <GatesEngine/Header/Util/Random.h>
#include <GatesEngine/Header/Graphics/Window.h>

Camera2D* Camera2D::GetInstance()
{
	static Camera2D instance;
	return &instance;
}

void Camera2D::Initialize()
{
	originPosition = position = { 0,0,0 };
	shakeFlag.Initialize();
	shakeState = true;
	countForShakeFlag = 0;
	shakePower = 1;

	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo(position, { 0,0,1 }, { 0,1,0 });
}

void Camera2D::Update(float deltaTime)
{
	UpdateShakeProcess(deltaTime);
}

void Camera2D::UpdateShakeProcess(float deltaTime)
{
	if (shakeFlag.GetFlag() == false)return;

	if (shakeFlag.GetOverTimeTrigger())
	{
		Initialize();
		return;
	}

	if (shakeState)
	{
		position += GE::Math::Vector3(GE::RandomMaker::GetFloat(-1, 1), GE::RandomMaker::GetFloat(-1, 1), 0) * shakePower;
	}
	else
	{
		position = originPosition;
	}

	// âΩÉtÉåÅ[ÉÄÇ…àÍìxâÊñ óhÇÍÇ≥ÇπÇÈÇ©
	const int SHAKE_FRAME = 4;
	if (countForShakeFlag % SHAKE_FRAME == 0)shakeState = !shakeState;
	++countForShakeFlag;
}

const GE::Math::Vector3& Camera2D::GetOriginPosition()
{
	return originPosition;
}

const GE::Math::Vector3& Camera2D::GetPosition()
{
	return position;
}

const GE::CameraInfo& Camera2D::GetCameraInfo()
{
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo(position, { 0,0,1 }, { 0,1,0 });
	return cameraInfo;
}

void Camera2D::Shake(float sec, float power)
{
	Initialize();
	shakeFlag.SetFlag(true);
	shakeFlag.SetMaxTimeProperty(sec);
	shakePower = power;
}

Camera2D::Camera2D()
	: originPosition()
	, position()
	, shakeFlag()
	, shakeState(true)
	, countForShakeFlag(0)
	, shakePower(1)
	, cameraInfo()
{
}
