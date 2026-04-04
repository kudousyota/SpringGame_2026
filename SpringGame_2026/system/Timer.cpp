#include "Timer.h"
#include "DxLib.h"

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
	m_gameTimer = 60;
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
	DrawFormatString(30,30,GetColor(255, 255, 255),"Timer:%d",m_gameTimer);
	if (m_gameTimer == 0)
	{
		DrawString(30, 50, "Time's up!", GetColor(255, 0, 0));
	}
}
