#pragma once
#include "../system/Vector3.h"
class Enemy
{
public:
	Enemy();
	~Enemy();
	void Init();
	void End();
	void Update();
	void Draw();
private:
	Vector3 m_pos;
};

