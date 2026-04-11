#include <algorithm>
#include <cmath>
#include "ResultScene.h"
#include "DxLib.h"
#include "../Game/Input.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Game.h"
#include "../Game/Player.h"

constexpr int fade_interval = 60;

void ResultScene::FadeInUpdate(Input& input)
{
	// フェードイン中でもエンター("ok")でフェードアウトを開始できるようにする
	if (input.IsTriggered("ok"))
	{
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		m_frame = 0;	// フェードアウトの最初
		return;
	}


	if (m_frame-- <= 0)
	{
		m_update = &ResultScene::NormalUpdate;
		m_draw = &ResultScene::NormalDraw;
		return;
	}
}



void ResultScene::NormalUpdate(Input& input)
{
	
	m_finalScore = Player::GetJustCount();

	if (input.IsTriggered("ok"))
	{
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		m_frame = 0;	// フェードアウトの最初
		return;
	}
}

void ResultScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= fade_interval)
	{
		// フェードアウト完了
		m_finished = true;
		return;
	}
}
void ResultScene::NormalDraw()
{
	DrawStringToHandle(320, 240, "Result Scene", GetColor(0, 0, 0), m_fontHandle);
	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);

	DrawFormatString2ToHandle(470, 400, Color, Cyan, m_fontHandle, "回避した回数: %d", m_finalScore);
	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		DrawStringToHandle(470, 580, "Press A to Title", white, m_fontHandle);
	}
}

void ResultScene::FadeDraw()
{
	//値の範囲を一旦0.0~1.0fにしておくといろいろと扱いやすい
	auto rate = static_cast<float>(m_frame) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}


ResultScene::ResultScene(SceneController& controller) :
	m_draw(0),
	m_fontHandle(-1),
	m_displayScore(0),
	m_finalScore(0),
	m_scoreAnimTime(0),
	m_finished(false),
	Scene(controller)
{
	m_update = &ResultScene::FadeInUpdate;
	m_draw = &ResultScene::FadeDraw;
	m_frame = fade_interval;
}
ResultScene::~ResultScene()
{
	//何もしない
}

void ResultScene::Init()
	
{
	// フォントの読み込み
	m_fontHandle = CreateFontToHandle("x10y12pxDonguriDuel", 40, -1, DX_FONTTYPE_ANTIALIASING);

}

void ResultScene::Update(Input& input)
{
	(this->*m_update)(input);
}
void ResultScene::Draw()
{
	
    (this->*m_draw)();

}
