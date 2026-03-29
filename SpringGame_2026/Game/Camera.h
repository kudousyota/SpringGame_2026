#pragma once
#include <memory>
#include "../system/Vector3.h"
class Player;
class Camera
{
public:
	Camera();
	~Camera();
	void Init();
	void End();
	void Update();
private:
	// プレイヤーへの参照を保持
	std::shared_ptr<Player> m_pPlayer; 
	// カメラの位置
	Vector3 m_cameraTarget;
	Vector3 m_cameraPos;
};

