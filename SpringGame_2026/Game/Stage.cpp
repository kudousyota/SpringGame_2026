#include "Stage.h"
#include "Dxlib.h"

Stage::Stage():
	m_stageMin(VGet(-900.0f, 0.0f, -500.0f)),
	m_stageMax(VGet(900.0f, 0.0f, 500.0f))
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
}

void Stage::End()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
}

bool Stage::CanMove(const Vector3& pos, float radius)const
{
	//プレイヤーの移動可能な範囲内にいるかどうかを判定する
    if (pos.x - radius < m_stageMin.x) return false;
    if (pos.x + radius > m_stageMax.x) return false;
    if (pos.z - radius < m_stageMin.z) return false;
    if (pos.z + radius > m_stageMax.z) return false;

    return true;
}
