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
}

void Enemy::End()
{
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
	//DrawSphere3D(m_pos.ToDxLibVector(), 20.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
}
