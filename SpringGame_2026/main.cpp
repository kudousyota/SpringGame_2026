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

	//描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);
	// カリングの設定
	SetUseBackCulling(true);

	// 入力とタイトルシーンを先に処理する
	Input input;
	TitleScene title;
	title.Init();


	// タイトルが終わるまでループ
	while (ProcessMessage() != -1 && !title.IsFinished())
	{
		//このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		//前のフレームに描画した内容をクリアする
		ClearDrawScreen();

		input.Update();

		title.Update(input);
		title.Draw();

		//escキーを押すとゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
		//描画した内容を画面に反映する
		ScreenFlip();

		//フレームレート６０に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{
		}
	}

	// タイトル終了後にメインシーンを作成
	std::shared_ptr<SceneMain> pScene = std::make_shared<SceneMain>();
	pScene->Init();

    bool timeUpTriggered = false;
	while (ProcessMessage() != -1)
	{
		//このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		//前のフレームに描画した内容をクリアする
		ClearDrawScreen();

		input.Update();

        //ここにゲームの処理を書く

		pScene->Update(input);

		pScene->Draw();

        // タイマーが切れたらリザルトシーンへ遷移
		if (pScene->IsTimeUp())
		{
			timeUpTriggered = true;
			break;
		}

		//escキーを押すとゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
		//描画した内容を画面に反映する
		ScreenFlip();

		//フレームレート６０に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{
		}

	}
	
    // タイムアップならリザルトシーンへ移行
	if (timeUpTriggered)
	{
		SceneController controller;
		ResultScene result(controller);
		// ResultScene のループ
		while (ProcessMessage() != -1 && !result.IsFinished())
		{
			LONGLONG start = GetNowHiPerformanceCount();
			ClearDrawScreen();
			input.Update();
			result.Update(input);
			result.Draw();
			if (CheckHitKey(KEY_INPUT_ESCAPE))
			{
				break;
			}
			ScreenFlip();
			while (GetNowHiPerformanceCount() - start < 16667)
			{
			}
		}
	}

	//メモリの解放

	
	//shared_ptrはメモリの開放を自動でやってくれる
	// DxLib_End() を呼ぶ前に Dx ライブラリを使う可能性のあるオブジェクトを破棄する
	// (デストラクタで MV1DeleteModel 等を呼ぶと DxLib_End() 後だとクラッシュする)
	pScene.reset();

    ::DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;
}