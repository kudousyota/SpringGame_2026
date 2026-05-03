#include "SoundManager.h"
#include <DxLib.h>
std::unordered_map<std::string, int> SoundManager::m_bgmList;
std::unordered_map<std::string, int> SoundManager::m_seList;
int SoundManager::m_currentBgm = -1;
SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::Load()
{
	//ここでBGMとSEのファイルを読み込んで、m_bgmListとm_seListにハンドルを保存する
	//例
	//m_seList["hit"] = LoadSoundMem("hit.wav");

	m_bgmList["Title"] = LoadSoundMem("Sound/Title.ogg");
	m_bgmList["Result"] = LoadSoundMem("Sound/Result.ogg");
	m_seList["Ok"] = LoadSoundMem("Sound/Ok.mp3");
	m_seList["Dodge"] = LoadSoundMem("Sound/Dodge.mp3");
	m_seList["Just"] = LoadSoundMem("Sound/Just.mp3");
	m_seList["Hit"] = LoadSoundMem("Sound/Hit.mp3");
	m_seList["CountDown"] = LoadSoundMem("Sound/CountDown.mp3");
}

void SoundManager::PlayBGM(const std::string& key, bool loop)
{
	//BGM再生処理
	//まず、鍵となる文字列からBGMのハンドルを取得する
	auto it = m_bgmList.find(key);
	//見つからなかった場合は何もしない
	if (it == m_bgmList.end())
	{
		return;
	}
	//現在再生中のBGMがあれば停止する
	if (m_currentBgm != -1)
	{
		StopSoundMem(m_currentBgm);
	}
	//新しいBGMを再生する
	m_currentBgm = it->second;
	if (m_currentBgm >= 0)
	{
		//音量調整
		ChangeVolumeSoundMem(100, m_currentBgm);
		//ループ再生の設定
		PlaySoundMem(m_currentBgm, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
	}
}

void SoundManager::PlaySE(const std::string& key)
{
	//SE再生処理
	//鍵となる文字列からSEのハンドルを取得する
	auto it = m_seList.find(key);
	
	if (it != m_seList.end())
	{
		ChangeVolumeSoundMem(170, it->second);
		//SEはループさせないので、DX_PLAYTYPE_BACKを指定して再生する
		PlaySoundMem(it->second, DX_PLAYTYPE_BACK);
	}
}

void SoundManager::StopBGM()
{
	//現在再生中のBGMがあれば停止する
	if (m_currentBgm != -1)
	{
		//BGMを停止する
		StopSoundMem(m_currentBgm);
		//現在再生中のBGMのハンドルをリセットする
		m_currentBgm = -1;
	}
}

void SoundManager::Update()
{
	//必要に応じて、BGMやSEの状態を更新する処理をここに書く
}
