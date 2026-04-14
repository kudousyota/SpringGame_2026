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
	std::shared_ptr<Effect> m_Effects;

	//エフェクトのハンドル
	int m_effectHandle;
    //再生中のエフェクトのハンドル（複数再生に対応）
	std::vector<int> m_playingEffectHandles;

	// 再生間隔用タイマー
	int m_playTimer;


};

