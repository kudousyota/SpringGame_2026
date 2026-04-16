#pragma once
#include "Vector3.h"

class Effect
{
public:
	Effect();
	~Effect();
	void Init();
	void Update();
	void Draw();
private:
	enum class EffectState
	{
		Stoped,//完全停止
		Playing,//再生中
	};

	Vector3 m_pos;
    //エフェクトリソースのハンドル
	int m_effectResourceHandle;
	//再生中のエフェクトハンドル
	int m_playingHandle;
};

