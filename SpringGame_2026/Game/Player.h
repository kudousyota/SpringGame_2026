#pragma once
#include "../system/Vector3.h"
class Input;
class Camera;
class Timer;
class Stage;
class Player
{
public:
	Player();
	~Player();

	void Init();
	void End();
	void Update(const Input& input ,const Timer& timer);
	void Draw();

	// プレイヤーが弾に当たったときの処理
	void OnHit();

	// プレイヤーの位置と向きを取得する関数
	Vector3 GetPos() const { return m_pos; }
	float GetAngle() const { return m_angle; }
	// カメラの注視点を取得する関数
	Vector3 GetCameraTarget()const;
	//当たり判定用
	Vector3 GetColCenter()const;
	float GetColRadius()const;
	//今無敵かどうか
	bool IsInvincible()const;
	

  void setCamera(Camera* camera) { m_pCamera = camera; }
	void setStage(Stage* stage) { m_pStage = stage; }

private:
	enum class Inputdata
	{
		None,
		Up,
		Down,
		Left,
		Right
	};
	// カメラへの参照
	Camera* m_pCamera;
	// ステージへの参照
	Stage* m_pStage;

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
	//ヒットアニメーションのフレーム
	int m_hitAnimFrame;

	//自分の半径
	float m_colRadius;

	//今被弾中かどうか
	bool m_isHit;

	//ヒットアニメーション中かどうか
	bool IsHitPlaying() const;
	
};

