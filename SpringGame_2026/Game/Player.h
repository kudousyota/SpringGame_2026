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

	// プレイヤーの位置と向きを取得する関数
	Vector3 getPos() const { return m_pos.ToDxLibVector(); }
	float getAngle() const { return m_angle; }
	// カメラの注視点を取得する関数
	Vector3 getCameraTarget()const;

	Vector3 GetColCenter()const;
	float GetColRadius()const;

private:
	enum class Inputdata
	{
		None,
		Up,
		Down,
		Left,
		Right
	};
	int m_modelHandle;

	Vector3 m_pos;

	float m_angle;

	//2回入力されたか
	bool m_isDoubleInput;
	float m_doubleInputTimer;
	Inputdata m_inputData;
	bool m_isInput;
	
	// 回避中か
	bool m_isDodge;
	// 回避の残り時間
	float m_dodgeTimer;
	//無敵時間
	float m_invincibleTimer;
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

