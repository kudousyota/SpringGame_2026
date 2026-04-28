#pragma once
#include <memory>
#include <vector>
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include "../system/Timer.h"
#include "../Game/Stage.h"
#include "../system/EffectManager.h"
#include "Scene.h"

class ResultScene;
class Stage;
class Input;
class Player;
class Enemy;
class Timer;
class SceneMain:public Scene
{
public:
	SceneMain(SceneController& controller);
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();
    bool IsTimeUp() const;

	

private:
	void DrawGrid();
	//フェードインupdate
	void FadeInUpdate(Input&);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(SceneMain::*)(Input&);
	UpdateFunc_t m_update;	// Update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void(SceneMain::*)();
	DrawFunc_t m_draw;	// Draw系を受け取るメンバ関数ポインタ

	//カウントダウン
	void CountDownUpdate(Input& input);
	void CountDownDraw();
	int m_countDownFrame;

	int m_fontHandle;
	int m_fontHandleTwo;
	int m_frameCount;
	int m_operationHandle;
	int m_shadowMapHandle;
	int m_frame;
	//bool m_isFinished;
	std::shared_ptr<Player> m_pPlayer;
	std::vector<std::shared_ptr<Enemy>>m_pEnemy;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Timer> m_pTimer;
    std::shared_ptr<Stage> m_pStage;
    std::shared_ptr<EffectManager> m_pEffect;
	
	

};

