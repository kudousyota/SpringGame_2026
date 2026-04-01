#include "Player.h"
#include "Dxlib.h"
#include "Input.h"
#include "Camera.h"

namespace
{
	constexpr float kspeed = 3.0f;
	//2回入力されたとみなす時間
	constexpr int kDoubleInputTime = 30;
	//回避速度
	constexpr float kDodgeSpeed = 30.0f;
	//回避時間
	constexpr float kDodgeTime = 10.0f;
    //回避後の無敵時間
    constexpr float kInvincibleTime = 60.0f;
    //アニメーションの名前
    const char* const kIdleAnimName = "CharacterArmature|Idle";
    const char* const kRunAnimName = "CharacterArmature|Run";
	//アニメーションの切り替えフレーム数
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
	//当たり判定用の半径
	m_colRadius = 30.0f;
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

void Player::Update(const Input& input)
{
    if (m_pCamera == nullptr) return;

    //ダブル入力判定用タイマー
    m_doubleInputTimer++;

    //無敵時間タイマー
    if (m_invincibleTimer > 0.0f)
    {
        m_invincibleTimer -= 1.0f;
        if (m_invincibleTimer < 0.0f) m_invincibleTimer = 0.0f;
    }

    Vector3 forward = m_pCamera->GetForward();
    Vector3 right = m_pCamera->GetRight();

    //入力方向取得
    Inputdata now = Inputdata::None;

    if (input.IsPressed("up"))    now = Inputdata::Up;
    else if (input.IsPressed("down"))  now = Inputdata::Down;
    else if (input.IsPressed("left"))  now = Inputdata::Left;
    else if (input.IsPressed("right")) now = Inputdata::Right;

    m_isInput = (now != Inputdata::None);

    //回避開始判定
    if (!m_isDodge && m_isInput)
    {
        bool triggered = false;

        if (now == Inputdata::Up)    triggered = input.IsTriggered("up");
        else if (now == Inputdata::Down)  triggered = input.IsTriggered("down");
        else if (now == Inputdata::Left)  triggered = input.IsTriggered("left");
        else if (now == Inputdata::Right) triggered = input.IsTriggered("right");

        if (triggered)
        {
            if (m_inputData == now && m_doubleInputTimer < kDoubleInputTime)
            {
                m_isDodge = true;
                m_dodgeTimer = kDodgeTime;
                m_invincibleTimer = kInvincibleTime;
            }

            m_inputData = now;
            m_doubleInputTimer = 0;
        }
    }

    //回避中処理
    if (m_isDodge)
    {
        Vector3 dodgeVec(0.0f, 0.0f, 0.0f);

        switch (m_inputData)
        {
        case Inputdata::Up:    dodgeVec = forward; break;
		case Inputdata::Down:  dodgeVec = forward * -1.0f; break;//前後はカメラの向きに沿って移動
		case Inputdata::Left:  dodgeVec = right * -1.0f; break;//左右はカメラの向きに沿って移動
        case Inputdata::Right: dodgeVec = right; break;
        default: break;
        }

        if (dodgeVec.SqMagnitude() > 0.0001f)
        {
            dodgeVec = dodgeVec.Normalize();
            m_pos += dodgeVec * kDodgeSpeed;
            m_angle = atan2f(dodgeVec.x, dodgeVec.z) + DX_PI_F;
        }

        m_dodgeTimer -= 1.0f;
        if (m_dodgeTimer <= 0.0f)
        {
            m_isDodge = false;
        }
    }
    else
    {
        //通常移動
        Vector3 moveVec(0.0f, 0.0f, 0.0f);

        if (input.IsPressed("up"))    moveVec += forward;
        if (input.IsPressed("down"))  moveVec -= forward;
        if (input.IsPressed("left"))  moveVec -= right;
        if (input.IsPressed("right")) moveVec += right;

        if (moveVec.SqMagnitude() > 0.0001f)
        {
            moveVec = moveVec.Normalize() * kspeed;
            m_pos += moveVec;
            m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
        }
    }

    //アニメーション切り替え
    bool isMoving = false;
    if (m_isDodge)
    {
        isMoving = true;
    }
    else
    {
        Vector3 checkMove(0.0f, 0.0f, 0.0f);
        if (input.IsPressed("up"))    checkMove += forward;
        if (input.IsPressed("down"))  checkMove -= forward;
        if (input.IsPressed("left"))  checkMove -= right;
        if (input.IsPressed("right")) checkMove += right;
        isMoving = (checkMove.SqMagnitude() > 0.0001f);
    }

    const char* desiredAnim = isMoving ? kRunAnimName : kIdleAnimName;
    int animNo = MV1GetAnimIndex(m_modelHandle, desiredAnim);

    if (animNo != m_currentAnimIndex)
    {
        if (m_lastAnimHandle != -1)
        {
            MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
        }

        m_lastAnimHandle = m_cureentAnimHandle;
        m_lastAnimCount = m_currentAnimCount;

        m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
        m_currentAnimIndex = animNo;
        m_currentAnimCount = 0.0f;
        m_animChangeFrame = 0;
    }

    //アニメブレンド
    m_animChangeFrame++;

    if (m_lastAnimHandle == -1)
    {
        MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, 1.0f);
    }
    else
    {
        float rate = static_cast<float>(m_animChangeFrame) / kAnimChangeFrame;
        if (rate > 1.0f) rate = 1.0f;

        MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, rate);
        MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnimHandle, 1.0f - rate);

        if (rate >= 1.0f)
        {
            MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
            m_lastAnimHandle = -1;
        }
    }

    //アニメ進行
    m_currentAnimCount += 0.5f;
    float currentTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);
    if (m_currentAnimCount >= currentTotal)
    {
        m_currentAnimCount -= currentTotal;
    }
    MV1SetAttachAnimTime(m_modelHandle, m_cureentAnimHandle, m_currentAnimCount);

    //Lastアニメ進行
    if (m_lastAnimHandle != -1)
    {
        m_lastAnimCount += 0.5f;
        float lastTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_lastAnimHandle);
        if (m_lastAnimCount >= lastTotal)
        {
            m_lastAnimCount -= lastTotal;
        }
        MV1SetAttachAnimTime(m_modelHandle, m_lastAnimHandle, m_lastAnimCount);
    }

    // モデル行列を毎フレーム更新
    MATRIX rotMtx = MGetRotY(m_angle);
    MATRIX transMtx = MGetTranslate(m_pos.ToDxLibVector());
    MATRIX world = MMult(rotMtx, transMtx);
    MV1SetMatrix(m_modelHandle, world);
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
    return m_pos + kPlayerToTarget;
}
//当たり判定用の中心を返す関数
Vector3 Player::GetColCenter() const
{
    return m_pos + VGet(0.0f,60.0f,0);
}

float Player::GetColRadius() const
{
    return m_colRadius;
}



