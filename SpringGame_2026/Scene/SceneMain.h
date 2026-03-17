#pragma once
#include <memory>
#include <vector>
#include "../Game/Player.h"

class Player;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update();
	void Draw();
private:
	void DrawGrid();

private:
	int m_frameCount;
	std::shared_ptr<Player> m_pPlayer;

};

