#pragma once
#include <unordered_map>
#include <string>
#include <memory>
class SoundManager
{
public:
	static void Load();
	//BGMを再生する関数。引数はBGMの鍵となる文字列
	static void PlayBGM(const std::string& key, bool loop);
	static void PlaySE(const std::string& key);
	static void StopBGM();
	static void Update();
private:

	//BGMとSEのハンドルを管理するマップ
	static std::unordered_map<std::string, int> m_bgmList;
	static std::unordered_map<std::string, int> m_seList;

	static int m_currentBgm; //現在再生中のBGMのハンドル
};

