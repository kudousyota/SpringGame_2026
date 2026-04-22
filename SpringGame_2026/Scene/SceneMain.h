#pragma once
#include <memory>
#include <vector>
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include "../system/Timer.h"
#include "../Game/Stage.h"
#include "../system/EffectManager.h"

class Stage;
class Input;
class Player;
class Enemy;
class Timer;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(const Input& input);
	void Draw();
    bool IsTimeUp() const;

	

private:
	void DrawGrid();

	int m_fontHandle;
	int m_frameCount;
	int m_operationHandle;

	std::shared_ptr<Player> m_pPlayer;
	std::vector<std::shared_ptr<Enemy>>m_pEnemy;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Timer> m_pTimer;
    std::shared_ptr<Stage> m_pStage;
    std::shared_ptr<EffectManager> m_pEffect;

	

};

