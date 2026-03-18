#pragma once
#include "../system/Vector3.h"
class Input;
class Player
{
public:
	Player();
	~Player();

	void Init();
	void End();
	void Update(Input input);
	void Draw();
private:
	int m_modelHandle;

	Vector3 m_pos;

	float m_abgle;


	

};

