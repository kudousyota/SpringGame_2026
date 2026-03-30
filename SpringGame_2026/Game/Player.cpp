#include "Player.h"
#include "Dxlib.h"
#include "Input.h"

namespace
{
	constexpr float kspeed = 3.0f;
	constexpr float kDoubleInputSpeed = 6.0f;
	//2回入力されたとみなす時間
	constexpr int kDoubleInputTime = 30;
	// 回避速度
	constexpr float kDodgeSpeed = 12.0f;
	// 回避時間
	constexpr float kDodgeTime = 10.0f;
    // 回避後の無敵時間
    constexpr float kInvincibleTime = 60.0f;
    //アニメーションの名前
    const char* const kIdleAnimName = "CharacterArmature|Idle";
    const char* const kRunAnimName = "CharacterArmature|Run";
	// アニメーションの切り替えフレーム数
    constexpr int kAnimChangeFrame = 9;
    //プレイヤー基準位置から注視点までのベクトル
    //プレイヤー位置から注視点に向かうベクトル
    const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);
}

Player::Player() :
	m_modelHandle(-1),
	m_angle(0.0f),
    m_isDoubleInput(false),
    m_doubleInputTimer(0.0f),
    m_inputData(Inputdata::None),
    m_isInput(false),
    m_isDodge(false),
    m_dodgeTimer(0.0f),
	m_invincibleTimer(0.0f),
    m_pos(VGet(0, 0, 0)),
    m_cureentAnimHandle(-1),
    m_lastAnimHandle(-1),
    m_currentAnimIndex(-1),
    m_animChangeFrame(0),
    m_lastAnimCount(0.0f),
    m_currentAnimCount(0.0f)
{
}
Player::~Player()
{
	
}

void Player::Init()
{
    m_modelHandle = MV1LoadModel("data/Player.mv1");

    int animNo = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
	m_currentAnimCount = 0.0f;

    m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
    m_currentAnimIndex = animNo;

    m_lastAnimHandle = -1;
    m_lastAnimCount = 0.0f;

    m_animChangeFrame = 0;
}

void Player::End()
{
    if (m_modelHandle != -1)
    {
        if (m_cureentAnimHandle != -1)
        {
            MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
            m_cureentAnimHandle = -1;
        }
        if (m_lastAnimHandle != -1)
        {
            MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
            m_lastAnimHandle = -1;
        }
        MV1DeleteModel(m_modelHandle);
        m_modelHandle = -1;
    }
}

void Player::Update(Input input)
{
    //0.5秒以内に同じ入力がされたらダッシュ
    m_doubleInputTimer++;

	//今の入力方向の初期化
	Inputdata now = Inputdata::None;

	if (input.IsPressed("up"))
	{
		now = Inputdata::Up;
	}
	if (input.IsPressed("down"))
	{
		now = Inputdata::Down;
	}
	if (input.IsPressed("left"))
	{
		now = Inputdata::Left;
	}
	if (input.IsPressed("right"))
	{
		now = Inputdata::Right;
	}
	
	m_isInput = (now != Inputdata::None);
	

    if (m_isDodge)
    {
        //回避速度
        float dodgeSpeed = kDodgeSpeed;  

        // 回避方向にステップ移動
        switch (m_inputData)
        {
        case Inputdata::Up:    m_pos.z += dodgeSpeed; break;
        case Inputdata::Down:  m_pos.z -= dodgeSpeed; break;
        case Inputdata::Left:  m_pos.x -= dodgeSpeed; break;
        case Inputdata::Right: m_pos.x += dodgeSpeed; break;
        }

        // 回避残り時間を減らす
        m_dodgeTimer -= 1.0f;

        // 回避終了
        if (m_dodgeTimer <= 0)
        {
            m_isDodge = false;
        }

        // モデル位置反映
        MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());
        return; //回避中は通常移動をさせない
    }
        //ダブルタップ回避開始
    if (m_isInput)
    {
        // ダブルタップ条件今フレームで入力がトリガーされた場合
        // IsTriggered を使うことで、押された瞬間のみを検出する
        bool triggered = false;
        if (now == Inputdata::Up) triggered = input.IsTriggered("up");
        if (now == Inputdata::Down) triggered = input.IsTriggered("down");
        if (now == Inputdata::Left) triggered = input.IsTriggered("left");
        if (now == Inputdata::Right) triggered = input.IsTriggered("right");

        if (m_inputData == now && triggered && m_doubleInputTimer < kDoubleInputTime)
        {
            // 回避開始
            m_isDodge = true;
            // 回避時間
            m_dodgeTimer = kDodgeTime;
            // 回避成功時に無敵時間を付与
            m_invincibleTimer = kInvincibleTime;
            // 回避方向を記憶
            m_inputData = now;
        }

        // 今回の入力を記憶しておく
        m_inputData = now;
        m_doubleInputTimer = 0;
    }
    // 通常移動 (移動量をベクトルで保持)
    float speed = kspeed;
    Vector3 moveVec(0.0f, 0.0f, 0.0f);

    if (now == Inputdata::Up)    moveVec.z += speed;
    if (now == Inputdata::Down)  moveVec.z -= speed;
    if (now == Inputdata::Left)  moveVec.x -= speed;
    if (now == Inputdata::Right) moveVec.x += speed;

    // 位置更新
    m_pos += moveVec;

    // モデルを移動方向に向ける (移動ベクトルがあるとき)
    if (moveVec.SqMagnitude() > 0.0001f)
    {
        // atan2f(x, z) によって、0 が +Z 方向を向くように角度を算出
        m_angle = atan2f(moveVec.x, moveVec.z);
    }

    // 無敵時間のカウントダウン
    if (m_invincibleTimer > 0.0f)
    {
        m_invincibleTimer -= 1.0f;
        if (m_invincibleTimer < 0.0f) m_invincibleTimer = 0.0f;
    }

    // 回転行列と平行移動行列を合成してモデルに反映
    MATRIX rotMtx = MGetRotY(m_angle);
    MATRIX transMtx = MGetTranslate(m_pos.ToDxLibVector());
    MATRIX world = MMult(rotMtx, transMtx);
    MV1SetMatrix(m_modelHandle, world);

    // アニメーションは切り替わったときだけAttach/Detachする
    // 今は移動していればRun、していなければIdleを再生する
    const char* desiredAnim = (moveVec.SqMagnitude() > 0.0001f) ? kRunAnimName : kIdleAnimName;
    int animNo = MV1GetAnimIndex(m_modelHandle, desiredAnim);
    if (animNo != m_currentAnimIndex)
    {
        // 古いアニメーションを外す
        if (m_cureentAnimHandle != -1)
        {
            MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
            m_cureentAnimHandle = -1;
        }
        // 新しいアニメーションをセット
        m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
        m_currentAnimIndex = animNo;
        m_currentAnimCount = 0.0f;
    }
    //アニメーションのブレンド率を変更して滑らかな切り替えを行う
	m_animChangeFrame++;
	if (m_lastAnimHandle = -1)
	{
		//アニメーションが1つしか設定されていない場合
		MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, 1.0f);
	}
	else
	{
		//currentAnimeのブレンド率を計算
		float rate = static_cast<float>(m_animChangeFrame) / kAnimChangeFrame;
		if (rate > 1.0f)rate = 1.0f;

		MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, rate);
		MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnimHandle, 1.0f - rate);
	}

    //アニメーションの進行
    m_currentAnimCount += 0.5f;
    //アニメーションのループ再生
    float totalCount = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);
    if (m_currentAnimCount >= totalCount)
    {
        m_currentAnimCount -= totalCount;
    }
    MV1SetAttachAnimTime(m_modelHandle, m_cureentAnimHandle, m_currentAnimCount);
    //もう一つのアニメーションも一応カウントを進める
    if (m_lastAnimHandle != -1)
    {
        m_lastAnimCount += 0.5f;
        //アニメーションのループ再生
        float totalCount = MV1GetAttachAnimTotalTime(m_modelHandle, m_lastAnimHandle);
        if (m_lastAnimCount >= totalCount)
        {
            m_lastAnimCount -= totalCount;
        }
        MV1SetAttachAnimTime(m_modelHandle, m_lastAnimHandle, m_lastAnimCount);

    }
}

void Player::Draw()
{
    // 無敵時間中は点滅表示、それ以外は通常描画
    bool shouldDrawModel = true;
    if (m_invincibleTimer > 0.0f)
    {
        const int blinkFrequency = 5; // 点滅の頻度
        int timer = static_cast<int>(m_invincibleTimer);
        bool show = ((timer / blinkFrequency) % 2) == 0;
        shouldDrawModel = show;
    }

    if (shouldDrawModel && m_modelHandle != -1)
    {
        MV1DrawModel(m_modelHandle);
    }

    // 当たり判定の可視化
    DrawSphere3D(m_pos.ToDxLibVector(), 200, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), false);

}

Vector3 Player::getCameraTarget() const
{
	Vector3 pos = m_pos;
    // カメラの注視点をプレイヤーの頭あたりに設定
	pos.y += 100.0f; 
	return VAdd(pos.ToDxLibVector(), kPlayerToTarget.ToDxLibVector());
}

