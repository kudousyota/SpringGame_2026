#pragma once
#include "Vector3.h"
#include <memory>
#include <vector>

class Effect;
class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void PlayHitEffect(const Vector3& pos);

private:
	std::shared_ptr<Effect> m_Effect;

	//エフェクトのハンドル
	int m_effectHandle;
    //再生中のエフェクトのハンドル
	std::vector<int> m_effects;

	//再生間隔用タイマー
	int m_playTimer;


};

