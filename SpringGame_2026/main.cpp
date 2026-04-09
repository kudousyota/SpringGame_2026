#include <DxLib.h>
#include "Game.h"

#include <memory> //シェアードポイント
#include "Game/Input.h"
#include "Scene/SceneMain.h"
#include "Scene/TitleScene.h"
#include "Scene/SceneController.h"
#include "Scene/ResultScene.h"
//定数定義
namespace
{
	//画面サイズ
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
}


// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//ウィンドウモード設定
	ChangeWindowMode(true);
	//タイトル変更
	SetMainWindowText("ゲーム名");
	//画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);
	if (DxLib_Init() == -1) 		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	//フォントの読み込み
	AddFontResourceEx("data/x10y12pxDonguriDuel.ttf",FR_PRIVATE,NULL);

	//描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);
	// カリングの設定
	SetUseBackCulling(true);

    // 入力はアプリ全体で一つ用意しておく
	Input input;

	bool running = true;

	while (running && ProcessMessage() != -1)
	{
		// タイトルを表示
		TitleScene title;
		title.Init();
		while (running && ProcessMessage() != -1 && !title.IsFinished())
		{
			LONGLONG start = GetNowHiPerformanceCount();
			ClearDrawScreen();
			input.Update();
			title.Update(input);
			title.Draw();
			if (CheckHitKey(KEY_INPUT_ESCAPE))
			{
				running = false;
				break;
			}
			ScreenFlip();
			while (GetNowHiPerformanceCount() - start < 16667) {}
		}

		if (!running) break;

		// メインシーンを作成して実行
		std::shared_ptr<SceneMain> pScene = std::make_shared<SceneMain>();
		pScene->Init();
		bool timeUpTriggered = false;

		while (running && ProcessMessage() != -1)
		{
			LONGLONG start = GetNowHiPerformanceCount();
			ClearDrawScreen();
			input.Update();
			pScene->Update(input);
			pScene->Draw();
			if (pScene->IsTimeUp())
			{
				timeUpTriggered = true;
				break;
			}
			if (CheckHitKey(KEY_INPUT_ESCAPE))
			{
				running = false;
				break;
			}
			ScreenFlip();
			while (GetNowHiPerformanceCount() - start < 16667) {}
		}

		// メインシーン解放
		pScene.reset();

		if (!running) break;

		// タイムアップならリザルトを表示してからタイトルに戻る
		if (timeUpTriggered)
		{
			SceneController controller;
			ResultScene result(controller);
			result.Init();
			while (running && ProcessMessage() != -1 && !result.IsFinished())
			{
				LONGLONG start = GetNowHiPerformanceCount();
				ClearDrawScreen();
				input.Update();
				result.Update(input);
				result.Draw();
				if (CheckHitKey(KEY_INPUT_ESCAPE))
				{
					running = false;
					break;
				}
				ScreenFlip();
				while (GetNowHiPerformanceCount() - start < 16667) {}
			}
			// リザルト終了後はループを継続してタイトルに戻る
		}
		else
		{
			// タイムアップでなく通常終了（ESCなど）の場合は終了
			break;
		}
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;
}