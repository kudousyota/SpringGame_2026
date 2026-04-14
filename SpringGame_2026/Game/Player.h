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
	// ジャスト回避が発生したときの処理
	void OnJustDodge();
    // ジャスト回避の回数を取得
	static int GetJustCount();
	// 回避中かどうか取得
	bool IsDodging()const;
	// ジャスト回避が成立するウィンドウにいるか
	bool IsInJustWindow() const;
	// ジャスト回避用の当たり判定半径を取得
	float GetJustColRadius() const;

	// 最近ジャストウィンドウにいたか（frames以内）
	bool WasInJustWindowRecently(int frames) const;

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

	enum class PlayerScene
	{
		Game,
		TitleScene,
		ResultScene
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

	PlayerScene m_scene;
	
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
	//ジャスト回避用の半径
	float m_justColRadius;
    //ジャスト回避のフレーム
	float m_justFrame;
	//ジャスト回避できた回数(全体で管理する静的カウンタ)
	static int s_justCount;
	// ジャスト判定に触れてからの経過フレーム（最近触れていたかを判定するため）
	int m_framesSinceJustWindow;
	
	//今被弾中かどうか
	bool m_isHit;

	//ヒットアニメーション中かどうか
	bool IsHitPlaying() const;
	
};

