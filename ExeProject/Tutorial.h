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
	//�G�����ʒu
	static const GE::Math::Vector3 POS_GENERATE_ENEMY_1;
	static const GE::Math::Vector3 POS_GENERATE_ENEMY_2;

	//FIRST_ATTACK�Ńv���C���[���~�܂点��ʒu
	static const float FIRST_PLAYER_POS_X;

	//SECOND_ATTACK�ŉe���~�܂点��ʒu
	static const float FIRST_SHADOW_POS_X;

	//SECOND_ATTACK�Ńv���C���[���~�܂点��ʒu
	static const float SECOND_PLAYER_POS_X;

	//SECOND_ATTACK�ŉe���~�܂点��ʒu
	static const float SECOND_SHADOW_POS_X;

	//THIRD_ATTACK�Ńv���C���[���~�܂点��ʒu
	static const float THIRD_PLAYER_POS_X;

	//THIRD_ATTACK�ŉe���~�܂点��ʒu
	static const float THIRD_SHADOW_POS_X;

	//FOURTH_ATTACK�Ńv���C���[���~�܂点��ʒu
	static const float FOURTH_PLAYER_POS_X;

	//FOURTH_ATTACK�ŉe���~�܂点��ʒu
	static const float FOURTH_SHADOW_POS_X;

	//PRE_START����`���[�g���A���I�������ăQ�[���Ɉڂ�܂ł̎���
	static const float PRE_START_TIME;

private:
	static TutorialState tutorialState;		//�`���[�g���A���̏��
	static float tutorialTimer;				//�ėp�^�C�}�[
	static int changeStateCount;			//�J�ڗp�J�E���^�[
	static bool attackable[2];				//�U���\�t���O (�v���C���[�Ɖe��2��)
	static GE::IGraphicsDeviceDx12* graphicsDevice;

private:
	static void Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name);

public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="isSkipTutorial">�`���[�g���A�����X�L�b�v���邩</param>
	static void Initialize(bool isSkipTutorial = false);
	
	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	/// <param name="deltaTime">����</param>
	static void UpdateTimer(float deltaTime);

	/// <summary>
	/// �`��
	/// </summary>
	static void Draw();

	/// <summary>
	/// �`���[�g���A���󋵂�J�ڂ�����J�E���^�[�����炷 0�ɂȂ������Ԃ������I�Ɏ��̂��̂ɂ���
	/// </summary>
	/// <param name="resetNum">�J�E���g��0�ɂȂ����ۂɂ��̒l�Ń��Z�b�g����</param>
	static void DecrementChangeStateCount(int resetNum);

	/// <summary>
	/// �`���[�g���A���󋵎擾
	/// </summary>
	/// <returns>�`���[�g���A����</returns>
	static const TutorialState& GetTutorialState() { return tutorialState; }

	/// <summary>
	/// �`���[�g���A�����I�����Ă��邩�擾
	/// </summary>
	/// <returns>�`���[�g���A�����I�����Ă��邩</returns>
	static bool IsEndTutorial() { return Tutorial::tutorialState == TutorialState::GAME_START; }

	/// <summary>
	/// �^�C�}�[�̒l�擾
	/// </summary>
	/// <returns>�^�C�}�[�̒l</returns>
	static float GetTutorialTimer() { return tutorialTimer; }

	/// <summary>
	/// �U���\���擾
	/// </summary>
	/// <returns>�U���\�t���O (�`���[�g���A���łȂ��Ƃ��͏펞true)</returns>
	static bool IsAttackable() { return (attackable[0] && attackable[1]) || IsEndTutorial(); }

	/// <summary>
	/// �`���[�g���A���󋵃Z�b�g
	/// </summary>
	/// <param name="state">�`���[�g���A����</param>
	static void SetTutorialState(const TutorialState& state) { Tutorial::tutorialState = state; }

	/// <summary>
	/// �^�C�}�[�̒l�Z�b�g
	/// </summary>
	/// <param name="timer">�^�C�}�[�̒l</param>
	static void SetTutoarialTimer(const float timer) { Tutorial::tutorialTimer = timer; }

	/// <summary>
	/// �U���\�t���O�Z�b�g
	/// </summary>
	/// <param name="attackable">�U���\�t���O</param>
	/// <param name="index">0...Player 1...�e</param>
	static void SetAttackable(bool attackable, int index) { Tutorial::attackable[index] = attackable; }

	/// <summary>
	/// GraphicsDevice�Z�b�g
	/// </summary>
	/// <param name="gDevice">GraphicsDevice</param>
	static void SetGraphicsDevice(GE::IGraphicsDeviceDx12* gDevice) { Tutorial::graphicsDevice = gDevice; }
};

