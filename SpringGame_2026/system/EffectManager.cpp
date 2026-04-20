#include "EffectManager.h"
#include "EffekseerForDXLib.h"

namespace
{
	//エフェクトのサイズ
	constexpr float keffectSize = 10.0f;
	//エフェクトの再生間隔
	constexpr int keffectPlayInterval = 30;
}

EffectManager::EffectManager():
	m_playTimer(0)
{
}

EffectManager::~EffectManager()
{
    //再生中のハンドルを停止または解放
	for (int handle : m_pEffects)
	{
		if (handle != -1)
		{
			StopEffekseer3DEffect(handle);
		}
	}
	m_pEffects.clear();

	//リソース解放
	for (int i = 0; i < EffectType::Max; ++i)
	{
		if (m_effectHandles[i] != -1)
		{
			DeleteEffekseerEffect(m_effectHandles[i]);
			m_effectHandles[i] = -1;
		}
	}
}

void EffectManager::Init()
{
	//エフェクトリソースを読み込む
	m_effectHandles[EffectType::Hit] = LoadEffekseerEffect("data/Hit2.efk",3.0f);
	m_effectHandles[EffectType::Just] = LoadEffekseerEffect("data/Just.efk", 3.0f);

}


void EffectManager::Update()
{
	Effekseer_Sync3DSetting();

	if (m_playTimer > 0)
	{
		m_playTimer--;
	}

	for (auto it = m_pEffects.begin(); it != m_pEffects.end(); )
	{
		//再生中のエフェクトのハンドルを取得
		int handle = *it;
		//再生が終了しているか確認
		if (handle == -1 || !IsEffekseer3DEffectPlaying(handle))
		{
			StopEffekseer3DEffect(handle);
			it = m_pEffects.erase(it);
		}
		//再生中なら次のエフェクトへ
		else
		{
			++it;
		}
	}

	UpdateEffekseer3D();
}


void EffectManager::Draw()
{
	//Effekseerの描画関数を呼び出す
	DrawEffekseer3D();
}


void EffectManager::PlayHitEffect(const Vector3& pos)
{
	if (m_effectHandles[EffectType::Hit] == -1) return;
	if (m_playTimer > 0) return;

	int playing = PlayEffekseer3DEffect(m_effectHandles[EffectType::Hit]);
	if (playing != -1)
	{
		m_pEffects.push_back(playing);
		SetPosPlayingEffekseer3DEffect(playing, pos.x, pos.y, pos.z);
		SetScalePlayingEffekseer3DEffect
		(
			playing,
			keffectSize,
			keffectSize,
			keffectSize
		);

		m_playTimer = keffectPlayInterval;
	}
}

