#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Util/Math/Vector3.h>
#include <GatesEngine/Header/Graphics/CBufferStruct.h>

class Camera2D
{
private:
	GE::Math::Vector3 originPosition;
	GE::Math::Vector3 position;

	// ��ʗh��p�t���O
	GE::FlagController shakeFlag;
	// ��ʗh��̏��
	// true �Ȃ烉���_���ȕ����Ɉړ�
	// false �Ȃ猳�̈ʒu�ɖ߂�
	bool shakeState;
	int countForShakeFlag;
	float shakePower;

	GE::CameraInfo cameraInfo;

	bool isUpdateCameraInfo;
public:
	static Camera2D* GetInstance();
	void Initialize();
	void Update(float deltaTime);

	const GE::Math::Vector3& GetOriginPosition();
	const GE::Math::Vector3& GetPosition();

	// Cbuffer�p�f�[�^��Ԃ�
	const GE::CameraInfo& GetCameraInfo();

	void Shake(float sec,float power = 1);

private:
	Camera2D();
	Camera2D(const Camera2D&) = delete;
	~Camera2D() = default;
	Camera2D& operator=(const Camera2D&) = delete;

	void UpdateShakeProcess(float deltaTime);
};
