#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <string>

class SpriteParticleComponent : public GE::Component
{
private:
	std::string textureName = "NotSetTexture";	//画像名
	int textureNum = 1;							//連番画像枚数
	float texChangeSpeed = 0.1f;				//画像切り替え間隔
	float lifeTime = 1.0f;						//表示時間

	GE::Math::Vector3 initPosition = {};		//初期位置
	GE::Math::Vector3 velocity = {};			//移動量
	GE::Math::Vector3 accel = {};				//加速量

	bool isStart = false;						//描画・再生開始したか
	bool isLoopAnime = false;					//アニメーションをループさせるか

	float moveTimer = 0;						//移動用タイマー
	float animeTimer = 0;						//アニメ切り替え用タイマー

	bool isStartDelay;
	float startDelay;
	float delayTimer;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// 初期化　全ての情報が生成時の状態になります
	/// </summary>
	void Initialize();

	/// <summary>
	/// 再生開始
	/// </summary>
	void StartAnime(bool isDelay = false,float delayTime = 0);

	/// <summary>
	/// タイマー更新
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// 位置更新
	/// </summary>
	void UpdatePos();

	void UpdateDelayTimer(float deltaTime);

	/// <summary>
	/// アニメーションに使用するテクスチャの名前指定
	/// 連番がある場合は 〇〇〇_0のようなフォーマットにし、名前指定には"_0"を省く。
	/// </summary>
	/// <param name="textureName">画像名</param>
	void SetTextureName(const std::string& textureName) { this->textureName = textureName; }

	/// <summary>
	/// 連番画像の枚数指定
	/// </summary>
	/// <param name="textureNum">連番画像の枚数</param>
	void SetTextureNum(int textureNum) { this->textureNum = textureNum; }

	/// <summary>
	/// 画像切り替えスピード指定
	/// </summary>
	/// <param name="texChangeSpeed">画像切り替えスピード</param>
	void SetTexChangeSpeed(float texChangeSpeed) { this->texChangeSpeed = texChangeSpeed; }

	/// <summary>
	/// 描画時間を指定
	/// </summary>
	/// <param name="lifeTime">描画時間</param>
	void SetLifeTime(float lifeTime) { this->lifeTime = lifeTime; }

	/// <summary>
	/// 移動量指定
	/// </summary>
	/// <param name="velocity">移動量</param>
	void SetVelocity(const GE::Math::Vector3& velocity) { this->velocity = velocity; }

	/// <summary>
	/// 加速量指定
	/// </summary>
	/// <param name="accel">加速量</param>
	void SetAccel(const GE::Math::Vector3& accel) { this->accel = accel; }

	/// <summary>
	/// アニメーションをループさせるか指定
	/// </summary>
	/// <param name="isLoopAnime">アニメーションをループさせるか</param>
	void SetIsLoopAnime(bool isLoopAnime) { this->isLoopAnime = isLoopAnime; }

	/// <summary>
	/// 画像の初期位置指定
	/// </summary>
	/// <param name="position">画像の初期位置</param>
	void SetInitPosition(const GE::Math::Vector3& initPosition) { this->initPosition = initPosition; }

	/// <summary>
	/// 画像の大きさ指定 (ピクセル単位)
	/// </summary>
	/// <param name="scale">画像の大きさ</param>
	void SetScale(const GE::Math::Vector3& scale) { transform->scale = scale; }
};

