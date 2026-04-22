#include "Timer.h"
#include "DxLib.h"
#include "../Game.h"
namespace
{
	//初期のフレーム数（秒数 * FPS）を取得
	static constexpr int kFps = 60;
	static constexpr int kTimeLimit = 20; //秒
}
Timer::Timer():
	m_gameTimer(0),
	m_fontHandle(-1)
{
}

Timer::~Timer()
{
}

void Timer::Init()
{
	//ゲームタイマーを設定
    m_gameTimer = kTimeLimit * kFps;
	m_fontHandle = CreateFontToHandle("x10y12pxDonguriDuel", 80, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
}

void Timer::End()
{
	if (m_fontHandle != -1)
	{
		DeleteFontToHandle(m_fontHandle);
		m_fontHandle = -1;
	}
}

void Timer::Update()
{
    //ゲームタイマーが0より大きいときだけ減らす
	if (m_gameTimer > 0)
	{
		m_gameTimer--;
	}
}

void Timer::Draw()
{
	//残り秒数に変換
	//切り上げ表示
	int displayTime = (m_gameTimer + kFps - 1) / kFps;

	DrawFormatStringToHandle(500, 30, GetColor(255, 255, 255), m_fontHandle, "Timer:%d", displayTime);

	if (m_gameTimer == 0)
	{
		DrawString(30, 50, "Time's up!", GetColor(255, 0, 0));
	}
}

bool Timer::IsTimeUp() const
{
	//ゲームタイマーが0以下なら時間切れ
	return m_gameTimer <= 0;
}

void Timer::SetTimer(int timer)
{
	m_gameTimer = timer;
}

int Timer::GetInitialFrames()
{
	return kTimeLimit * kFps;
}
