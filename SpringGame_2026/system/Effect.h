#pragma once
#include "Vector3.h"

class Effect
{
public:
	Effect();
	~Effect();
	void Init();
	void Update();
	void Draw();
private:
	Vector3 m_pos;
	int m_effectHandle;
};

