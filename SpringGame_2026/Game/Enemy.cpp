#include "Enemy.h"
#include "Player.h"
#include "Dxlib.h"

namespace
{
	//弾の速度
	constexpr float kBulletSpeed = 5.0f;
}

Enemy::Enemy() :
	m_pos(VGet(300.0f, 30.0f, 300.0f)),
	m_bulletPos(VGet(0, 0, 0)),
	m_bulletDir(VGet(0, 0, 0)),
	m_bulletStartPos(VGet(0, 0, 0)),
	m_hasBullet(false),
	m_shotTimer(0)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
    // 初期位置はコンストラクタで設定済み
}

void Enemy::End()
{
}

void Enemy::Update(const Player& player)
{
	// プレイヤーの位置を取得
	Vector3 playerPos = player.GetPosition();

	// クールタイムを更新
	m_shotTimer++;

	// 弾が無く、一定時間たったら発射
	if (!m_hasBullet && m_shotTimer >= 60)
	{
		m_hasBullet = true;
		m_bulletPos = m_pos;
		m_bulletStartPos = m_pos;
		m_bulletDir = (playerPos - m_pos).Normalize();
		m_shotTimer = 0;
	}

	// 弾を進める
	if (m_hasBullet)
	{
		m_bulletPos += m_bulletDir * kBulletSpeed;

        // 一定距離進んだら消す（ルート計算を避けるため二乗距離で判定）
		float distSq = (m_bulletPos - m_bulletStartPos).SqMagnitude();
		constexpr float kMaxDist = 500.0f;
		if (distSq > kMaxDist * kMaxDist)
		{
			m_hasBullet = false;
		}
	}
}

void Enemy::Draw()
{
    // 見やすいサイズの球で敵を描画
	DrawSphere3D(m_pos.ToDxLibVector(), 50.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);

	// 弾を描画
	if (m_hasBullet)
	{
		DrawSphere3D(m_bulletPos.ToDxLibVector(), 10.0f, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
	}
}
