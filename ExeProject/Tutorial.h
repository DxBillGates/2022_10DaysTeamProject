#pragma once
#include <GatesEngine/Header/Util/Math/Vector3.h>
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>

enum class TutorialState {
	FIRST_ATTACK,
	SECOND_ATTACK,
	THIRD_ATTACK,
	FOURTH_ATTACK,
	PRE_START,
	GAME_START
};

class Tutorial
{
public:
	//敵生成位置
	static const GE::Math::Vector3 POS_GENERATE_ENEMY_1;
	static const GE::Math::Vector3 POS_GENERATE_ENEMY_2;

	//FIRST_ATTACKでプレイヤーを止まらせる位置
	static const float FIRST_PLAYER_POS_X;

	//SECOND_ATTACKで影を止まらせる位置
	static const float FIRST_SHADOW_POS_X;

	//SECOND_ATTACKでプレイヤーを止まらせる位置
	static const float SECOND_PLAYER_POS_X;

	//SECOND_ATTACKで影を止まらせる位置
	static const float SECOND_SHADOW_POS_X;

	//THIRD_ATTACKでプレイヤーを止まらせる位置
	static const float THIRD_PLAYER_POS_X;

	//THIRD_ATTACKで影を止まらせる位置
	static const float THIRD_SHADOW_POS_X;

	//FOURTH_ATTACKでプレイヤーを止まらせる位置
	static const float FOURTH_PLAYER_POS_X;

	//FOURTH_ATTACKで影を止まらせる位置
	static const float FOURTH_SHADOW_POS_X;

	//PRE_STARTからチュートリアル終了させてゲームに移るまでの時間
	static const float PRE_START_TIME;

private:
	static TutorialState tutorialState;		//チュートリアルの状態
	static float tutorialTimer;				//汎用タイマー
	static int changeStateCount;			//遷移用カウンター
	static bool attackable[2];				//攻撃可能フラグ (プレイヤーと影の2つ)
	static GE::IGraphicsDeviceDx12* graphicsDevice;

private:
	static void Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name);

public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="isSkipTutorial">チュートリアルをスキップするか</param>
	static void Initialize(bool isSkipTutorial = false);
	
	/// <summary>
	/// タイマー更新
	/// </summary>
	/// <param name="deltaTime">時間</param>
	static void UpdateTimer(float deltaTime);

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// チュートリアル状況を遷移させるカウンターを減らす 0になったら状態を自動的に次のものにする
	/// </summary>
	/// <param name="resetNum">カウントが0になった際にこの値でリセットする</param>
	static void DecrementChangeStateCount(int resetNum);

	/// <summary>
	/// チュートリアル状況取得
	/// </summary>
	/// <returns>チュートリアル状況</returns>
	static const TutorialState& GetTutorialState() { return tutorialState; }

	/// <summary>
	/// チュートリアルが終了しているか取得
	/// </summary>
	/// <returns>チュートリアルが終了しているか</returns>
	static bool IsEndTutorial() { return Tutorial::tutorialState == TutorialState::GAME_START; }

	/// <summary>
	/// タイマーの値取得
	/// </summary>
	/// <returns>タイマーの値</returns>
	static float GetTutorialTimer() { return tutorialTimer; }

	/// <summary>
	/// 攻撃可能か取得
	/// </summary>
	/// <returns>攻撃可能フラグ (チュートリアルでないときは常時true)</returns>
	static bool IsAttackable() { return (attackable[0] && attackable[1]) || IsEndTutorial(); }

	/// <summary>
	/// チュートリアル状況セット
	/// </summary>
	/// <param name="state">チュートリアル状況</param>
	static void SetTutorialState(const TutorialState& state) { Tutorial::tutorialState = state; }

	/// <summary>
	/// タイマーの値セット
	/// </summary>
	/// <param name="timer">タイマーの値</param>
	static void SetTutoarialTimer(const float timer) { Tutorial::tutorialTimer = timer; }

	/// <summary>
	/// 攻撃可能フラグセット
	/// </summary>
	/// <param name="attackable">攻撃可能フラグ</param>
	/// <param name="index">0...Player 1...影</param>
	static void SetAttackable(bool attackable, int index) { Tutorial::attackable[index] = attackable; }

	/// <summary>
	/// GraphicsDeviceセット
	/// </summary>
	/// <param name="gDevice">GraphicsDevice</param>
	static void SetGraphicsDevice(GE::IGraphicsDeviceDx12* gDevice) { Tutorial::graphicsDevice = gDevice; }
};

