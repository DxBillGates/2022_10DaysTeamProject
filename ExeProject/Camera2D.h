#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Util/Math/Vector3.h>
#include <GatesEngine/Header/Graphics/CBufferStruct.h>

class Camera2D
{
private:
	GE::Math::Vector3 originPosition;
	GE::Math::Vector3 position;

	// 画面揺れ用フラグ
	GE::FlagController shakeFlag;
	// 画面揺れの状態
	// true ならランダムな方向に移動
	// false なら元の位置に戻る
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

	// Cbuffer用データを返す
	const GE::CameraInfo& GetCameraInfo();

	void Shake(float sec,float power = 1);

private:
	Camera2D();
	Camera2D(const Camera2D&) = delete;
	~Camera2D() = default;
	Camera2D& operator=(const Camera2D&) = delete;

	void UpdateShakeProcess(float deltaTime);
};
