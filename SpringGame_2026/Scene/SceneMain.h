#pragma once
#include <memory>
#include <vector>
#include "../Game/Player.h"

class Input;
class Player;
class Enemy;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input input);
	void Draw();
private:
	void DrawGrid();

private:
	int m_frameCount;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Enemy>  m_pEnemy;

};

