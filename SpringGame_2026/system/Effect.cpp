#include "Effect.h"
#include "EffekseerForDXLib.h"
#include "../Game.h"

Effect::Effect():
	m_pos({ 0.0f,0.0f,0.0f }),
    m_effectResourceHandle(-1),
	m_playingHandle(-1)
{
}

Effect::~Effect()
{
    //停止とリソース解放
	if (m_playingHandle != -1)
	{
		StopEffekseer3DEffect(m_playingHandle);
		m_playingHandle = -1;
	}

	if (m_effectResourceHandle != -1)
	{
		DeleteEffekseerEffect(m_effectResourceHandle);
		m_effectResourceHandle = -1;
	}
}

void Effect::Init()
{
   
	EffectState effectState = EffectState::Stoped;

	//初期位置: 画面中央
	m_pos = VGet(Game::kScreenWidth * 0.5f, Game::kScreenHeight * 0.5f, 0.0f);
}

void Effect::Update()
{
	if (m_effectResourceHandle == -1) return;

	

	//再生中でなければ再生を開始する
	if (m_playingHandle == -1)
	{
		m_playingHandle = PlayEffekseer3DEffect(m_effectResourceHandle);
	}

	//再生中なら位置を設定
	if (m_playingHandle != -1)
	{
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
	}

	
}

void Effect::Draw()
{
	
}
