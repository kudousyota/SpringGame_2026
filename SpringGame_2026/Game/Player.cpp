#include "Player.h"
#include "Dxlib.h"
#include "Input.h"
#include "Camera.h"
#include "../system/Timer.h"
#include "Stage.h"

namespace
{
	//移動速度
	constexpr float kspeed = 5.0f;
	//2回入力されたとみなす時間
	constexpr int kDoubleInputTime = 30;
	//回避速度
	constexpr float kDodgeSpeed = 30.0f;
	//回避時間
	constexpr int kDodgeTime = 10;
    //回避後の無敵時間
    constexpr int kInvincibleTime = 90;
    //アニメーションの名前
    const char* const kIdleAnimName = "CharacterArmature|Idle";
    const char* const kRunAnimName = "CharacterArmature|Run";
    const char* const kHitAnimName = "CharacterArmature|HitReact";
	//アニメーションの切り替えフレーム数
    constexpr int kAnimChangeFrame = 9;
    //プレイヤー基準位置から注視点までのベクトル
    //プレイヤー位置から注視点に向かうベクトル
    const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);

    //点滅の頻度
	const int kBlinkFrequency = 5;

}


Player::Player() :
    m_pCamera(nullptr),
    m_pStage(nullptr),
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
    m_hitAnimFrame(0),
    m_colRadius(0.0f),
    m_justColRadius(0.0f),
    m_justFrame(0.0f),
    m_isHit(false),
    m_currentAnimCount(0.0f),
	m_scene(PlayerScene::Game),
    m_framesSinceJustWindow(100000)
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
	m_colRadius = 100.0f;
    //ジャスト回避が成立するフレーム(回避中の最初の数フレームを想定)
    m_justFrame = 6.0f;
    //ジャストカウントは静的カウンタで管理
    m_justCount = 0;
    //初期値: 何フレーム経過しているかは大きな値にしておく(最近触れていない)
    m_framesSinceJustWindow = 100000;
	//ジャスト回避の半径(当たり判定より少しでかくする)
    m_justColRadius = 130.0f;

    //被弾アニメーションのフレーム数
    m_hitAnimFrame = 0;
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
void Player::Update(const Input& input, const Timer& timer)
{
    //タイマーが0秒になったら動けなくする
    if (timer.IsTimeUp())
    {
        return;
    }

    if (m_pCamera == nullptr) return;

    //ダブル入力判定用タイマー
    m_doubleInputTimer++;

    //アニメブレンド
    m_animChangeFrame++;

    //無敵時間タイマー
    if (m_invincibleTimer > 0.0f)
    {
        m_invincibleTimer -= 1.0f;
        if (m_invincibleTimer < 0.0f)
        {
            m_invincibleTimer = 0.0f;
        }
    }

    //被弾アニメーションのタイマー
    if (m_hitAnimFrame > 0)
    {
        m_hitAnimFrame--;
    }

    Vector3 forward = m_pCamera->GetForward();
    Vector3 right = m_pCamera->GetRight();

    //入力方向取得
    Inputdata now = Inputdata::None;
    if (input.IsPressed("up"))         now = Inputdata::Up;
    else if (input.IsPressed("down"))  now = Inputdata::Down;
    else if (input.IsPressed("left"))  now = Inputdata::Left;
    else if (input.IsPressed("right")) now = Inputdata::Right;

    m_isInput = (now != Inputdata::None);

    //回避開始判定
    if (!m_isDodge && m_isInput)
    {
        bool triggered = false;
		//現在の入力が新たに押されたものか判定
        if (now == Inputdata::Up)         triggered = input.IsTriggered("up");
        else if (now == Inputdata::Down)  triggered = input.IsTriggered("down");
        else if (now == Inputdata::Left)  triggered = input.IsTriggered("left");
        else if (now == Inputdata::Right) triggered = input.IsTriggered("right");

        //同じ方向が短時間で2回押されたら回避
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

    //移動処理
    bool isMoving = false;

    if (m_isDodge)
    {
        Vector3 dodgeVec(0.0f, 0.0f, 0.0f);

        switch (m_inputData)
        {
        case Inputdata::Up:    dodgeVec = forward; break;
        case Inputdata::Down:  dodgeVec = forward * -1.0f; break;
        case Inputdata::Left:  dodgeVec = right * -1.0f; break;
        case Inputdata::Right: dodgeVec = right; break;
        default: break;
        }
		//回避ベクトルの正規化と移動
        if (dodgeVec.SqMagnitude() > 0.0001f)
        {
            dodgeVec = dodgeVec.Normalize();
            Vector3 nextPos = m_pos + dodgeVec * kDodgeSpeed;

            if (m_pStage == nullptr || m_pStage->CanMove(nextPos, m_colRadius))
            {
                m_pos = nextPos;
            }
			//回避方向に向く
            m_angle = atan2f(dodgeVec.x, dodgeVec.z) + DX_PI_F;
            isMoving = true;
        }

        m_dodgeTimer -= 1.0f;
        if (m_dodgeTimer <= 0.0f)
        {
            m_isDodge = false;
        }
    }
    else
    {
        Vector3 moveVec(0.0f, 0.0f, 0.0f);

        if (input.IsPressed("up"))
        {
            moveVec += forward;
        }
        if (input.IsPressed("down"))
        {
            moveVec -= forward;
        }
        if (input.IsPressed("left"))
        {
            moveVec -= right;
        }
        if (input.IsPressed("right"))
        {
            moveVec += right;
        }
   
		//移動ベクトルの正規化と移動
        if (moveVec.SqMagnitude() > 0.0001f)
        {
            moveVec = moveVec.Normalize();
            Vector3 nextPos = m_pos + moveVec * kspeed;
			//ステージの移動可能範囲内か判定してから移動
            if (m_pStage == nullptr || m_pStage->CanMove(nextPos, m_colRadius))
            {
                m_pos = nextPos;
            }

            m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
            isMoving = true;
        }
    }

    //アニメーション切り替え
    const char* desiredAnim = nullptr;

    //被弾中はHitアニメを最優先
    if (m_hitAnimFrame > 0)
    {
        desiredAnim = kHitAnimName;
    }
    else
    {
        desiredAnim = isMoving ? kRunAnimName : kIdleAnimName;
    }

    int animNo = MV1GetAnimIndex(m_modelHandle, desiredAnim);

    //アニメーションが変わるときだけ処理
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
	//アニメーションのブレンド
    if (m_lastAnimHandle == -1)
    {
        MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, 1.0f);
    }
	//Lastアニメがあるときはフレーム数に応じてブレンド
    else
    {
        float rate = static_cast<float>(m_animChangeFrame) / kAnimChangeFrame;
        if (rate > 1.0f) rate = 1.0f;

        MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, rate);
        MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnimHandle, 1.0f - rate);
		//ブレンドが完了したらLastアニメをデタッチ
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

    //モデル行列を毎フレーム更新
    MATRIX rotMtx = MGetRotY(m_angle);
    MATRIX transMtx = MGetTranslate(m_pos.ToDxLibVector());
    MATRIX world = MMult(rotMtx, transMtx);
    MV1SetMatrix(m_modelHandle, world);
}

void Player::Draw()
{
	
	//被弾したら点滅
    if(m_hitAnimFrame > 0)
    {
        //点滅の頻度で描画するかしないかを切り替える
        if ((m_hitAnimFrame / kBlinkFrequency) % 2 == 0)
        {
            MV1DrawModel(m_modelHandle);
        }
    }
    else
    {
        //被弾していないときは通常通り描画
        MV1DrawModel(m_modelHandle);
    }

    
#ifdef _DEBUG
    //当たり判定の可視化
    DrawSphere3D(GetColCenter().ToDxLibVector(), m_colRadius, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), false);

	//ジャスト回避用の当たり判定の可視化
  DrawSphere3D(GetColCenter().ToDxLibVector(), m_justColRadius, 10, GetColor(0, 255, 0), GetColor(0, 255, 0), false);

#endif

}

void Player::OnHit()
{
	//無敵中は当たらない
    if (IsInvincible())return;

    //被弾アニメの継続時間をセット
	m_hitAnimFrame = 30;
    //フラグを立てる
    m_isHit = true;

    //Hitアニメに切り替え
    int animNo = MV1GetAnimIndex(m_modelHandle, kHitAnimName);
    if (animNo >= 0 && animNo != m_currentAnimIndex)
    {
        if (m_lastAnimHandle != -1)
        {
            MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
        }
		//現在のアニメをLastにしてから新しいアニメをセット
        m_lastAnimHandle = m_cureentAnimHandle;
        m_lastAnimCount = m_currentAnimCount;
		//新しいアニメをセット
        m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
        m_currentAnimIndex = animNo;
        m_currentAnimCount = 0.0f;
        m_animChangeFrame = 0;
    }
	
}

Vector3 Player::GetCameraTarget() const
{
    return m_pos + kPlayerToTarget;
}
//当たり判定用の中心を返す関数
Vector3 Player::GetColCenter() const
{
    return m_pos + VGet(0.0f,80.0f,0);
}

float Player::GetColRadius() const
{
    return m_colRadius;
}

bool Player::IsInvincible() const
{
    return m_invincibleTimer > 0.0f;
}

bool Player::IsHitPlaying() const
{
    return m_hitAnimFrame > 0;
}

void Player::OnJustDodge()
{
    m_justCount++;
    //発動時は短時間無敵にする
    m_invincibleTimer = 30.0f;

}

int Player::m_justCount = 0;

int Player::GetJustCount()
{
    return m_justCount;
}

bool Player::IsDodging()const
{
    return m_isDodge;
}

bool Player::IsInJustWindow() const
{
    //ジャスト回避は回避開始から数フレームだけ成立する
    if (!m_isDodge) return false;
    return m_dodgeTimer > (kDodgeTime - m_justFrame);
}

bool Player::WasInJustWindowRecently(int frames) const
{
    return m_framesSinceJustWindow <= frames;
}

float Player::GetJustColRadius() const
{
    return m_justColRadius;
}



