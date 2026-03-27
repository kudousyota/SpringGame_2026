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
	Vector3 m_bulletStartPos;

	bool m_hasBullet;

	//クールタイム
	int m_shotTimer;

	
};

