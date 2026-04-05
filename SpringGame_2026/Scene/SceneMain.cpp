#include "SceneMain.h"
#include <DxLib.h>
#include "../Game/Player.h"
#include "../Game/Enemy.h"
#include "../Game/Input.h"

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
	if (m_pEnemy)
	{
		m_pEnemy->End();
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

	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init();

	m_pEnemy = std::make_shared<Enemy>();
	m_pEnemy->Init();

	m_pCamera = std::make_shared<Camera>();
	m_pCamera->SetPlayer(m_pPlayer);
	m_pCamera->Init();

	
	m_pPlayer->setCamera(m_pCamera.get());

	m_pTimer = std::make_shared<Timer>();
	m_pTimer->Init();

	// 背景色を設定(ここでRGBを変更すれば背景色を変えれる）
	SetBackgroundColor(100, 150, 255); // 薄い青
	
}

void SceneMain::Update(const Input& input)
{
	m_frameCount++;
    if (m_pPlayer)
	{
     // Timerは m_pTimerを内部で保持しているため、Player::Update のシグネチャに合わせて呼び出す
		if (m_pTimer)
		{
			m_pTimer->Update();
			m_pPlayer->Update(input, *m_pTimer);
		}
		else
		{
			// タイマーが無ければ通常の更新を行う（時間切れフラグは false とみなす）
			Timer dummy;
			// dummy を初期化してから渡す
			dummy.Init();
			m_pPlayer->Update(input, dummy);
		}
	}
    if (m_pEnemy && m_pPlayer)
	{
		m_pEnemy->Update(*m_pPlayer);
	}
	if (m_pCamera)
	{
		m_pCamera->Update();
	}
    // m_pTimer の更新は Player 更新時にまとめて行うためここでは行わない
	

}

void SceneMain::Draw()
{
	// 画面を背景色でクリアしてから描画する
	ClearDrawScreen();

	if (m_pPlayer)
	{
		m_pPlayer->Draw();
	}
	if (m_pEnemy)
	{
		m_pEnemy->Draw();
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
