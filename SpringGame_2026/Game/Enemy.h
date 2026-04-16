#pragma once
#include "../system/Vector3.h"
class Player;
class Timer;
class EffectManager;
class Enemy
{
public:
	Enemy();
	~Enemy();
	void Init();
	void End();
    void Update(Player& player, const Timer& timer, EffectManager& effectManager);
	void Draw();

	//敵の位置を外部から設定する
	void SetPos(const Vector3& pos);

	

private:
	//モデルのハンドル
	int m_modelHandle;
	//敵の弾のモデルのハンドル
	int m_bulletModelHandle;
	Vector3 m_pos;
	float m_angle;
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
	//現在セットしているアニメーションのインデックス
	int m_currentAnimIndex;
	//アニメーション切り替えフレーム数
	int m_animChangeFrame;
	//攻撃アニメのフレーム
	int m_BiteAnimeFrame;
	//攻撃中かどうか
	bool m_isAttac;
	//弾のスピード
	float m_bulletSpeed;
	//攻撃頻度
	int m_attackCoolTime;

public:
	//攻撃頻度(初期クールタイム)をフレームで設定
	void SetAttackCoolTime(int frames) { m_attackCoolTime = frames; }
	//弾の速度を設定
	void SetBulletSpeed(float speed) { m_bulletSpeed = speed; }
};

