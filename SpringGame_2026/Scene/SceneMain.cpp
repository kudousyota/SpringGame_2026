#include "SceneMain.h"
#include <DxLib.h>
#include "../Game/Player.h"
#include "../Game/Enemy.h"
#include "../Game/Input.h"
#include "../Game/Stage.h"
#include "../system/EffectManager.h"

SceneMain::SceneMain():
m_frameCount(0)
{
}



SceneMain::~SceneMain()
{
	// PlayerとEnemy の終了処理を呼び出す
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

	
}
void SceneMain::Init()
{
	

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700.0f), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(200.0f, 2000.0f);

   // ステージを作成して初期化
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();

    // プレイヤーと敵を作成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init();

    // Use EffectManager to play hit effects
	m_pEffect = std::make_shared<EffectManager>();
	m_pEffect->Init();

	// 複数の敵をvectorで管理する
	// 例として3体作成して初期位置をそれぞれずらす
	// 既存の敵をクリア
	m_pEnemy.clear();
	auto enemy = std::make_shared<Enemy>();
	enemy->Init();
	// 位置を設定
	Vector3 pos = VGet(-200.0f, 150.0f, 450.0f);
	enemy->SetPos(pos);
	m_pEnemy.push_back(enemy);
	// 2体目
	auto enemyTwo = std::make_shared<Enemy>();
	enemyTwo->Init();
    // 2体目は攻撃が速め
	enemyTwo->SetAttackCoolTime(45);
	enemyTwo->SetBulletSpeed(12.0f);
	Vector3 posTwo = VGet(-700.0f, 150.0f, -350.0f);
	enemyTwo->SetPos(posTwo);
	m_pEnemy.push_back(enemyTwo);
	// 3体目
	auto enemyThree = std::make_shared<Enemy>();
	enemyThree->Init();
    // 3体目は攻撃が遅め
	enemyThree->SetAttackCoolTime(80);
	enemyThree->SetBulletSpeed(8.0f);
	Vector3 posThree = VGet(700.0f, 150.0f, -350.0f);
	enemyThree->SetPos(posThree);
	m_pEnemy.push_back(enemyThree);

	// カメラを作成しプレイヤーをセット
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->SetPlayer(m_pPlayer);
	m_pCamera->Init();

	// プレイヤーにカメラとステージを渡す
	m_pPlayer->setCamera(m_pCamera.get());
	m_pPlayer->setStage(m_pStage.get());

	m_pTimer = std::make_shared<Timer>();
	m_pTimer->Init();

	// 背景色を設定(ここでRGBを変更すれば背景色を変えれる)
	// 薄い青
	SetBackgroundColor(100, 150, 255); 
	
}

void SceneMain::Update(const Input& input)
{
	m_frameCount++;
    if (m_pPlayer)
	{
		// Timerはm_pTimerを内部で保持しているため、PlayerのUpdateのシグネチャに合わせて呼び出す
		if (m_pTimer)
		{
			m_pTimer->Update();
			m_pPlayer->Update(input, *m_pTimer);
		}
		else
		{
			// タイマーが無ければ通常の更新を行う（時間切れフラグはfalseとみなす）
			Timer dummy;
			// dummy を初期化してから渡す
			dummy.Init();
			m_pPlayer->Update(input, dummy);
		}
	}

    if (!m_pEnemy.empty() && m_pPlayer)
	{
        for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{
                // Pass effect manager to enemy update so it can play hit effects
				enemy->Update(*m_pPlayer, *m_pTimer, *m_pEffect);
			}
		}
	}
	// カメラの更新はPlayerの更新後に行う
	if (m_pCamera)
	{
		m_pCamera->Update();
	}
    // m_pTimerの更新はPlayer更新時にまとめて行うためここでは行わない
	
	//エフェクトを更新
	if (m_pEffect)
	{
		m_pEffect->Update();
	}

}

void SceneMain::Draw()
{
	// 画面を背景色でクリアしてから描画する
	ClearDrawScreen();

    if (m_pPlayer)
	{
		m_pPlayer->Draw();
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
	//エフェクトを描画
	if (m_pEffect)
	{
		m_pEffect->Draw();
	}
	if (m_pTimer)
	{
		m_pTimer->Draw();
	}
    
#ifdef _DEBUG

	DrawGrid();
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
#endif // _DEBUG

	
}

void SceneMain::DrawGrid()
{
	// 直線の始点と終点
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

bool SceneMain::IsTimeUp() const
{
	if (m_pTimer)
	{
		return m_pTimer->IsTimeUp();
	}
	return false;
}