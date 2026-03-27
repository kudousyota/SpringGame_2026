#pragma once
#include "../system/Vector3.h"
class Player;

class Enemy
{
public:
	Enemy();
	~Enemy();
	void Init();
	void End();
    void Update(const Player& player);
	void Draw();
private:
	Vector3 m_pos;
	//弾
	Vector3 m_bulletPos;
	Vector3 m_bulletDir;
	bool m_hasBullet;

	
	
};

