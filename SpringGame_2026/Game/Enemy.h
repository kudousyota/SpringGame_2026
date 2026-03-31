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
	int m_modelHandle;
	Vector3 m_pos;
	//弾
	Vector3 m_bulletPos;
	Vector3 m_bulletDir;
	Vector3 m_bulletStartPos;

	bool m_hasBullet;

	//クールタイム
	int m_shotTimer;

	//アニメーション
	float m_currentAnimCount;
	float m_lastAnimCount;
	//アニメーションのハンドル
	int m_cureentAnimHandle;
	int m_lastAnimHandle;
	// 現在セットしているアニメーションのインデックス
	int m_currentAnimIndex;
	//アニメーション切り替えフレーム数
	int m_animChangeFrame;
};

