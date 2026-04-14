#include "EffectManager.h"
#include "EffekseerForDXLib.h"

namespace
{
	//エフェクトのサイズ
	constexpr float keffectSize = 10.0f;
	//エフェクトの再生間隔
	constexpr int keffectPlayInterval = 300;
}

EffectManager::EffectManager():
	m_effectHandle(-1),
    m_playTimer(0)
{
}

EffectManager::~EffectManager()
{
    // 再生中のハンドルを停止または解放
	for (int handle : m_playingEffectHandles)
	{
		if (handle != -1)
		{
			StopEffekseer3DEffect(handle);
		}
	}
	m_playingEffectHandles.clear();

	// リソース解放
	if (m_effectHandle != -1)
	{
		DeleteEffekseerEffect(m_effectHandle);
		m_effectHandle = -1;
	}
}

void EffectManager::Init()
{
	// エフェクトリソースを読み込む
	m_effectHandle = LoadEffekseerEffect("data/Hit.efk",3.0f);

}

void EffectManager::Update()
{

	// 3D描画設定を同期
	Effekseer_Sync3DSetting();

	// 再生間隔のタイマーをデクリメント
	if (m_playTimer > 0)
	{
		m_playTimer--;
	}

	//Effekseerにより再生中のエフェクトを更新する
    UpdateEffekseer3D();

	// 再生終了したハンドルを削除
	for (auto Handleit = m_playingEffectHandles.begin(); Handleit != m_playingEffectHandles.end(); )
	{
		int handle = *Handleit;
		if (handle == -1)
		{
			Handleit = m_playingEffectHandles.erase(Handleit);
			continue;
		}

		int playing = IsEffekseer3DEffectPlaying(handle);
		if (playing == -1)
		{
			// 再生終了している
			Handleit = m_playingEffectHandles.erase(Handleit);
		}
		else
		{
			++Handleit;
		}
	}
}

void EffectManager::Draw()
{
	//Effekseerの描画関数を呼び出す
	DrawEffekseer3D();
}

void EffectManager::PlayHitEffect(const Vector3& pos)
{
    if (m_effectHandle == -1) return;

	// 再生間隔が残っている場合は再生しない
	if (m_playTimer > 0) return;

	

	// リソースを使って再生(戻り値を複数保持しておく)
	int playing = PlayEffekseer3DEffect(m_effectHandle);
	if (playing != -1)
	{
		m_playingEffectHandles.push_back(playing);
		// 位置を設定
		SetPosPlayingEffekseer3DEffect(playing, pos.x, pos.y, pos.z);

		SetScalePlayingEffekseer3DEffect(playing, keffectSize, keffectSize, keffectSize);

		// 再生間隔タイマーをリセット
		m_playTimer = keffectPlayInterval;
	}
}
