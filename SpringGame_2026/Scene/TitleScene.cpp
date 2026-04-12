#include "TitleScene.h"
#include "DxLib.h"
#include "../Game.h"


constexpr int fade_interval = 60;

void TitleScene::FadeInUpdate(Input&)
{
	if (m_frame-- <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
		return;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("ok"))
	{
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_frame = 0;	// フェードアウトの最初
		return;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= fade_interval)
	{
		// フェードアウト完了
		m_finished = true;
		return;
	}
}

void TitleScene::NormalDraw()
{
    const int cx = Game::kScreenWidth / 2;
	const int cy = Game::kScreenHeight / 2;

	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);

	DrawStringToHandle(510, 330 , "Title Scene", GetColor(0, 0, 0),m_fontHandle);

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

void TitleScene::FadeDraw()
{

	// 値の範囲をいったん0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = static_cast<float>(m_frame) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,255 * rate); // αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);	// 画面全体に黒フィルムをかける
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	// ブレンドしない
}

TitleScene::TitleScene():
	m_fontHandle(-1)
{
	
	m_update = &TitleScene::FadeInUpdate;
	m_draw = &TitleScene::FadeDraw;
	m_frame = fade_interval;
    m_finished = false;
}

void TitleScene::Init()
{
	SetBackgroundColor(100, 150, 255);
	//フォントの読み込み
	m_fontHandle = CreateFontToHandle("x10y12pxDonguriDuel", 40, -1, DX_FONTTYPE_ANTIALIASING);
}

void TitleScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}