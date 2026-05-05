#include "SceneMain.h"
#include <DxLib.h>
#include "../Game/Player.h"
#include "../Game/Enemy.h"
#include "../Game/Input.h"
#include "../Game/Stage.h"
#include "../system/EffectManager.h"
#include "ResultScene.h"
#include "../Game.h"
#include "SceneController.h"
#include "../system/SoundManager.h"

namespace
{
	constexpr int kFadeInterval = 60;
}

SceneMain::SceneMain(SceneController& controller):
Scene(controller),
m_frameCount(0),
m_fontHandle(-1),
m_operationHandle(-1),
m_fontHandleTwo(-1),
m_countDownFrame(0)
{
	m_update = &SceneMain::FadeInUpdate;
	m_draw = &SceneMain::FadeDraw;
	m_frame = kFadeInterval;
}



SceneMain::~SceneMain()
{
	

	//PlayerとEnemy の終了処理を呼び出す
    if (m_pPlayer)
	{
		m_pPlayer->End();
	}
   if (!m_pEnemy.empty())
	{
		for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{
				enemy->End();
			}
		}
		m_pEnemy.clear();
	}
    if (m_pStage)
	{
		m_pStage->End();
	}

	DeleteShadowMap(m_shadowMapHandle);
	
}
void SceneMain::Init()
{
	

	//Zバッファの設定
	SetUseZBuffer3D(true);	//Zバッファを使います
	SetWriteZBuffer3D(true);	//描画する物体はZバッファにも距離を書き込む

	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700.0f), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(20.0f, 10000.0f);

   //ステージを作成して初期化
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();

    //プレイヤーと敵を作成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init();

    //Use EffectManager to play hit effects
	m_pEffect = std::make_shared<EffectManager>();
	m_pEffect->Init();


	//複数の敵をvectorで管理する
	//例として3体作成して初期位置をそれぞれずらす
	//既存の敵をクリア
	m_pEnemy.clear();
	auto enemy = std::make_shared<Enemy>();
	enemy->Init();
	//位置を設定
	Vector3 pos = VGet(-200.0f, 150.0f, 550.0f);
	enemy->SetPos(pos);
	m_pEnemy.push_back(enemy);
	//2体目
	auto enemyTwo = std::make_shared<Enemy>();
	enemyTwo->Init();
    //2体目は攻撃が速め
	enemyTwo->SetAttackCoolTime(45);
	enemyTwo->SetBulletSpeed(12.0f);
	Vector3 posTwo = VGet(-1000.0f, 150.0f, -350.0f);
	enemyTwo->SetPos(posTwo);
	m_pEnemy.push_back(enemyTwo);
	//3体目
	auto enemyThree = std::make_shared<Enemy>();
	enemyThree->Init();
    //3体目は攻撃が遅め
	enemyThree->SetAttackCoolTime(80);
	enemyThree->SetBulletSpeed(8.0f);
	Vector3 posThree = VGet(1000.0f, 150.0f, -350.0f);
	enemyThree->SetPos(posThree);
	m_pEnemy.push_back(enemyThree);

	//カメラを作成しプレイヤーをセット
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->SetPlayer(m_pPlayer);
	m_pCamera->Init();

	//プレイヤーにカメラとステージを渡す
	m_pPlayer->SetCamera(m_pCamera.get());
	m_pPlayer->SetStage(m_pStage.get());

	m_pTimer = std::make_shared<Timer>();
	m_pTimer->Init();

	//フォントのサイズを大きくして見やすくする
	//CreateFontToHandle("x10y12pxDonguriDuel", 40, -1, DX_FONTTYPE_ANTIALIASING);

	m_fontHandle = CreateFontToHandle("x10y12pxDonguriDuel", 40, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
	m_fontHandleTwo = CreateFontToHandle("x10y12pxDonguriDuel", 160, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
	m_operationHandle = LoadGraph("data/Operation.png");

	//4秒
	m_countDownFrame = 240;

	m_shadowMapHandle = MakeShadowMap(8192, 8192);

	

	//背景色を設定(ここでRGBを変更すれば背景色を変えれる)
	//薄い青
	//SetBackgroundColor(100, 150, 255); 
	
}

void SceneMain::Update(Input& input)
{
	(this->*m_update)(input);
}

void SceneMain::Draw()
{
	(this->*m_draw)();
}

void SceneMain::DrawGrid()
{
	//直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	for (int z = -300; z <= 300; z += 100)
	{
		startPos = VGet(-300.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(300.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = -300; x <= 300; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -300.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 300.0f);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
}

void SceneMain::FadeInUpdate(Input& input)
{
	if (input.IsTriggered("ok"))
	{
		m_update = &SceneMain::FadeOutUpdate;
		m_draw = &SceneMain::FadeDraw;
		m_frame = 0;
		return;
	}

	if (m_frame-- <= 0)
	{
		m_update = &SceneMain::CountDownUpdate;
		m_draw = &SceneMain::CountDownDraw;
		SoundManager::PlaySE("CountDown");
	}
}

void SceneMain::NormalUpdate(Input& input)
{
	m_frameCount++;
	if (m_pPlayer)
	{
		//Timerはm_pTimerを内部で保持しているため、PlayerのUpdateのシグネチャに合わせて呼び出す
		if (m_pTimer)
		{
			m_pTimer->Update();
			m_pPlayer->Update(input, *m_pTimer);
		}
		else
		{
			//タイマーが無ければ通常の更新を行う(時間切れフラグはfalseとみなす)
			Timer dummy;
			//dummy を初期化してから渡す
			dummy.Init();
			m_pPlayer->Update(input, dummy);
		}
	}
	if (m_pTimer && m_pTimer->IsTimeUp())
	{
		m_update = &SceneMain::FadeOutUpdate;
		m_draw = &SceneMain::FadeDraw;
		m_frame = 0;
	}

	if (!m_pEnemy.empty() && m_pPlayer)
	{
		for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{

				enemy->Update(*m_pPlayer, *m_pTimer, *m_pEffect);
			}
		}
	}
	//カメラの更新はPlayerの更新後に行う
	if (m_pCamera)
	{
		m_pCamera->Update();
	}
	//m_pTimerの更新はPlayer更新時にまとめて行うためここでは行わない

	//エフェクトを更新
	if (m_pEffect)
	{
		m_pEffect->Update();
	}

}

void SceneMain::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		
		//m_isFinished = true;
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));
		return;
	}
}

void SceneMain::FadeDraw()
{
	NormalDraw();

	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneMain::NormalDraw()
{
	//画面を背景色でクリアしてから描画する
	//ClearDrawScreen();
	//操作説明を描画


	m_pCamera->Draw();

	// シャドウマップにプレイヤーの影を書き込む
	ShadowMap_DrawSetup(m_shadowMapHandle);
	m_pPlayer->Draw();
	ShadowMap_DrawEnd();

	// シャドウマップを使用してステージを描画
	SetUseShadowMap(0, m_shadowMapHandle);
	m_pStage->Draw();
	SetUseShadowMap(0, -1);

	// プレイヤー本体を描画
	m_pPlayer->Draw();

	if (m_pEffect)
	{
		m_pEffect->Draw();
	}

	if (!m_pEnemy.empty())
	{
		for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{
				enemy->Draw();
			}
		}
	}


	//タイマーを描画
	if (m_pTimer)
	{
		m_pTimer->Draw();
	}

	//ジャスト回避回数の表示
	DrawFormatStringToHandle(10, 10, GetColor(255, 255, 255), m_fontHandle, "Just Dodge Count: %d", Player::GetJustCount());
	DrawRotaGraph(240, 600, 1.0f, 0.0f, m_operationHandle, true);


#ifdef _DEBUG

	DrawGrid();
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
#endif //_DEBUG


}

void SceneMain::CountDownUpdate(Input& input)
{
	m_countDownFrame--;
	if (m_countDownFrame < 0)
	{
		m_update = &SceneMain::NormalUpdate;
		m_draw = &SceneMain::NormalDraw;
	}
}

void SceneMain::CountDownDraw()
{
	//ゲーム画面は見えてるまま
	NormalDraw(); 

	int Timer = m_countDownFrame / 60;

	if (Timer > 0)
	{
		DrawFormatStringToHandle(600, 300, GetColor(255, 255, 255), m_fontHandleTwo, "%d", Timer);
		
	}
	else
	{
		DrawStringToHandle(580, 300, "GO!", GetColor(255, 255, 255), m_fontHandleTwo);
	}
	
}

bool SceneMain::IsTimeUp() const
{
	if (m_pTimer)
	{
		return m_pTimer->IsTimeUp();
	}
	return false;
}