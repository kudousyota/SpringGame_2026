#include "Enemy.h"
#include "Player.h"
#include "Dxlib.h"

namespace
{
	//弾の速度
	constexpr float kBulletSpeed = 5.0f;
}

Enemy::Enemy():
    m_pos(VGet(300.0f, 30.0f, 300.0f)),
	m_bulletPos(VGet(0,0,0)),
	m_bulletDir(VGet(0,0,0)),
	m_hasBullet(false)

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

	// まだ弾が無ければ発射する
	if (!m_hasBullet)
	{
		m_hasBullet = true;
		m_bulletPos = m_pos;
		m_bulletDir = (playerPos - m_pos).Normalize();
	}

	// 弾を進める
	if (m_hasBullet)
	{
		m_bulletPos += m_bulletDir * kBulletSpeed;
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
