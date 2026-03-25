#include "Player.h"
#include "Dxlib.h"
#include "Input.h"

namespace
{
	constexpr float kspeed = 3.0f;
	constexpr float kdoubleInputSpeed = 6.0f;
	//2回入力されたとみなす時間
	constexpr int kdoubleInputTime = 30;
	// 回避速度
	constexpr float kdodgeSpeed = 12.0f;
	// 回避時間
	constexpr float kdodgeTime = 10.0f;
    // 回避後の無敵時間（フレーム）
    constexpr float kinvincibleTime = 60.0f;
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
    m_pos(VGet(0, 0, 0))

{
}
Player::~Player()
{
	
}

void Player::Init()
{
	m_modelHandle = MV1LoadModel("data/Player.mv1");
}

void Player::End()
{
	MV1DeleteModel(m_modelHandle);
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
        float dodgeSpeed = kdodgeSpeed;  

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
    //  ダブルタップ回避開始
    if (m_isInput)
    {
        // ダブルタップ条件今フレームで入力がトリガーされた場合
        // IsTriggered を使うことで、押された瞬間のみを検出する
        bool triggered = false;
        if (now == Inputdata::Up) triggered = input.IsTriggered("up");
        if (now == Inputdata::Down) triggered = input.IsTriggered("down");
        if (now == Inputdata::Left) triggered = input.IsTriggered("left");
        if (now == Inputdata::Right) triggered = input.IsTriggered("right");

        if (m_inputData == now && triggered && m_doubleInputTimer < kdoubleInputTime)
        {
            // 回避開始
            m_isDodge = true;
            // 回避時間
            m_dodgeTimer = kdodgeTime;
            // 回避成功時に無敵時間を付与
            m_invincibleTimer = kinvincibleTime;
            // 回避方向を記憶
            m_inputData = now;
        }

        // 今回の入力を記憶しておく
        m_inputData = now;
        m_doubleInputTimer = 0;
    }
    //通常移動
    float speed = kspeed;

    if (now == Inputdata::Up)    m_pos.z += speed;
    if (now == Inputdata::Down)  m_pos.z -= speed;
    if (now == Inputdata::Left)  m_pos.x -= speed;
    if (now == Inputdata::Right) m_pos.x += speed;

	MV1SetPosition(m_modelHandle,m_pos.ToDxLibVector());

    // 無敵時間のカウントダウン
    if (m_invincibleTimer > 0.0f)
    {
        m_invincibleTimer -= 1.0f;
        if (m_invincibleTimer < 0.0f) m_invincibleTimer = 0.0f;
    }

}

void Player::Draw()
{
    // 無敵時間中は点滅表示、それ以外は通常描画
    bool shouldDrawModel = true;
    if (m_invincibleTimer > 0.0f)
    {
        const int blinkFrequency = 5; // 点滅の頻度（フレーム）
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

