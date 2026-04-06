#pragma once
#include "../system/Vector3.h"

class Vector3;
class Stage
{
public:
	Stage();
	~Stage();
	void Init();
	void End();
	void Update();
	void Draw();

	//プレイヤーがその位置に進めるかどうかを判定する関数
	bool CanMove(const Vector3& pos ,float radius)const;
private:
	//移動可能な範囲
	Vector3 m_stageMin;
	//移動可能な範囲
	Vector3 m_stageMax;


};
