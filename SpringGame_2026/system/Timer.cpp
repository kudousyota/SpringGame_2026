#include "Timer.h"
#include "DxLib.h"

namespace
{
	//ゲームのフレームレート
	constexpr int kFps = 60;

	//制限時間
	constexpr int kTlimeLimit = 60;
}

Timer::Timer():
	m_gameTimer(0)
{
}

Timer::~Timer()
{
}

void Timer::Init()
{
	//ゲームタイマーを60秒に設定
	m_gameTimer = kTlimeLimit * kFps;
}

void Timer::End()
{
}

void Timer::Update()
{
    // ゲームタイマーが0より大きいときだけ減らす
	if (m_gameTimer > 0)
	{
		m_gameTimer--;
	}
}

void Timer::Draw()
{
	// 残り秒数に変換
	// 切り上げ表示
	int displayTime = (m_gameTimer + kFps - 1) / kFps;

	DrawFormatString(30, 30, GetColor(255, 255, 255), "Timer:%d", displayTime);

	if (m_gameTimer == 0)
	{
		DrawString(30, 50, "Time's up!", GetColor(255, 0, 0));
	}
}
