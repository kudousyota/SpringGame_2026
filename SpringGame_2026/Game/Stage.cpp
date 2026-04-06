#include "Stage.h"
#include "Dxlib.h"

Stage::Stage():
	m_stageMin(VGet(-1000.0f, 0.0f, -1000.0f)),
	m_stageMax(VGet(1000.0f, 0.0f, 1000.0f))
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

void Stage::CanMove(const Vector3& pos, float radius)
{
	if (pos.x - radius < m_stageMin.x)
	{
		pos.x = m_stageMin.x + radius;
	}
	else if(pos.x + radius > m_stageMax.x)
	{
		pos.x = m_stageMax.x - radius;
	}
}
