#include "Enemy.h"

Enemy::Enemy():
	m_pos(VGet(300.0f, 0.0f, -300.0f))
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

void Enemy::Update()
{
}

void Enemy::Draw()
{
    // 見やすいサイズの球で敵を描画
	DrawSphere3D(m_pos.ToDxLibVector(), 50.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
}
