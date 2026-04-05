#include "Enemy.h"
#include "Player.h"
#include "Dxlib.h"

namespace
{
	//弾の速度
	constexpr float kBulletSpeed = 8.0f;

	//アニメーションの名前
	const char* const kIdleAnimName = "MonsterArmature|Idle";
	const char* const kBiteAnimName = "MonsterArmature|Bite_Front";

	// 弾の半径（描画時と同じ値）
	constexpr float kBulletRadius = 10.0f;

	//弾のサイズ
	constexpr float kBulletSize = 50.0f;

	// 敵が弾を発射してから消えるまでの最大距離
	constexpr float kBulletMaxDistance = 1000.0f;
}

Enemy::Enemy() :
	m_modelHandle(-1),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletPos(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletDir(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletStartPos(VGet(0.0f, 0.0f, 0.0f)),
	m_hasBullet(false),
	m_shotTimer(0),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_lastAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_animChangeFrame(0),
	m_angle(0.0f),
	m_BiteAnimeFrame(0),
	m_isAttac(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	m_modelHandle = MV1LoadModel("data/Enemy.mv1");

	// 敵の初期位置を設定
	m_pos = VGet(0.0f, 500.0f, 350.0f);
	MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());

	int animNo = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
	m_currentAnimCount = 0.0f;

	m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
	m_currentAnimIndex = animNo;

	m_lastAnimHandle = -1;
	m_lastAnimCount = 0.0f;

	m_animChangeFrame = 0;
}

void Enemy::End()
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

void Enemy::Update(Player& player)
{
	// プレイヤーの位置を取得
	Vector3 playerPos = player.getPos();

	// クールタイムを更新
	m_shotTimer++;

	// 弾が無く、一定時間たったら発射
	if (!m_hasBullet && !m_isAttac && m_shotTimer >= 60)
	{
		m_hasBullet = true;
		m_bulletPos = m_pos;
		m_bulletStartPos = m_pos;
		m_bulletDir = (playerPos - m_pos).Normalize();
		m_shotTimer = 0;

		m_isAttac = true;

		// 攻撃アニメーションを開始
		// Biteアニメに切り替え
		int animNo = MV1GetAnimIndex(m_modelHandle, kBiteAnimName);
		if (animNo >= 0 && animNo != m_currentAnimIndex)
		{
			// 今のアニメを外す
			if (m_cureentAnimHandle != -1)
			{
				MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
			}

			// Biteアニメをセット
			m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
			m_currentAnimIndex = animNo;
			m_currentAnimCount = 0.0f;
			m_animChangeFrame = 0;
		}
	}

	// 弾を進める
	if (m_hasBullet)
	{
		m_bulletPos += m_bulletDir * kBulletSpeed;

		// 衝突判定: プレイヤーとの当たり判定（球と球の衝突）
		// プレイヤーの中心と半径を取得
		Vector3 playerCenter = player.GetColCenter();
		float playerRadius = player.GetColRadius();

		float bulletRadius = kBulletRadius;

		// 距離の二乗で判定
		float hitDist = playerRadius + bulletRadius;
		float hitDistSq = hitDist * hitDist;
		float distSq = (m_bulletPos - playerCenter).SqMagnitude();
        if (distSq <= hitDistSq)
		{
			
			//無敵時間中は当たらない
			if (!player.IsInvincible())
			{
				// プレイヤー側の被弾処理を呼ぶ
				player.OnHit();
				// プレイヤーに当たったら弾を消す
				m_hasBullet = false;
			}
			
		}

		// 一定距離進んだら消すルート計算を避けるため二乗距離で判定
		constexpr float kMaxDist = kBulletMaxDistance;
		float travelDistSq = (m_bulletPos - m_bulletStartPos).SqMagnitude();
		if (travelDistSq > kMaxDist * kMaxDist)
		{
			m_hasBullet = false;
		}
        // アニメ切り替えは発射時に行う。ここでは継続時間の管理のみ
	}
	// アニメーション
	// アニメ進行
	m_currentAnimCount += 0.5f;
	float currentTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);

	// Bite中はループさせない
	if (m_currentAnimIndex == MV1GetAnimIndex(m_modelHandle, kBiteAnimName))
	{
		if (m_currentAnimCount >= currentTotal)
		{
			m_currentAnimCount = currentTotal;
		}
	}
	else
	{
		// Idleなどはループ
		if (m_currentAnimCount >= currentTotal)
		{
			m_currentAnimCount -= currentTotal;
		}
	}

	MV1SetAttachAnimTime(m_modelHandle, m_cureentAnimHandle, m_currentAnimCount);


	// Biteアニメが最後まで再生されたら Idle に戻す
	if (m_isAttac && m_currentAnimIndex == MV1GetAnimIndex(m_modelHandle, kBiteAnimName))
	{
		float biteTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);

		if (m_currentAnimCount >= biteTotal)
		{
			int idleAnimNo = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
			if (idleAnimNo >= 0 && idleAnimNo != m_currentAnimIndex)
			{
				// 今のBiteアニメを外す
				if (m_cureentAnimHandle != -1)
				{
					MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
				}

				// Idleアニメをセット
				m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, idleAnimNo, -1, -1);
				m_currentAnimIndex = idleAnimNo;
				m_currentAnimCount = 0.0f;
				m_animChangeFrame = 0;
			}

			m_isAttac = false;
		}
	}
}

void Enemy::Draw()
{
	// 敵のモデルを描画
	MV1DrawModel(m_modelHandle);
	// 弾を描画
	if (m_hasBullet)
	{
		DrawSphere3D(m_bulletPos.ToDxLibVector(), kBulletSize, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
	}
}
