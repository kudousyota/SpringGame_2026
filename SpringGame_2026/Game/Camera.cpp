#include "Camera.h"
#include "DxLib.h"
#include "Player.h"

namespace 
{
	//注視点からカメラに向かうベクトル
	const Vector3 kTargetToCamera = VGet(0.0f, 250.0f, 1100.0f);

	// 注視点からカメラまでの距離
	constexpr float kCameraDistance = 1100.0f;
	constexpr float kCameraHeight = 250.0f;

    // 補間(一時的に1.0fにして追従を即時にすることで動作確認しやすくする)
	constexpr float kCameraFollow = 1.0f;

	// スティック感度
	constexpr float kCameraRotSpeed = 0.00005f;

	// デッドゾーン
	constexpr int kStickDeadZone = 1000;


}

Camera::Camera():
	m_cameraAngleX(0.0f),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraTarget(VGet(0.0f, 0.0f, 0.0f))
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	Vector3 cameraTarget = m_pPlayer->getCameraTarget();
	float playerAng = m_pPlayer->getAngle();

	// 初期角度はプレイヤーの向きに合わせる
	m_cameraAngleX = playerAng;

	// カメラの初期位置
	Vector3 offset;
	offset.x = sinf(m_cameraAngleX) * kCameraDistance;
	offset.y = kCameraHeight;
	offset.z = cosf(m_cameraAngleX) * kCameraDistance;

	m_cameraTarget = cameraTarget;
	m_cameraPos = cameraTarget - offset;

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(),m_cameraTarget.ToDxLibVector());
}

void Camera::End()
{
}

void Camera::Update()
{

	int xBuf = 0, yBuf = 0;

	// プレイヤーがセットされているか確認
	if (!m_pPlayer)
	{
		DrawFormatString(0, 32, 0xff0000, "Camera: No player assigned");
		return;
	}

	// 右スティック入力取得
	GetJoypadAnalogInputRight(&xBuf, &yBuf, DX_INPUT_PAD1);

	DrawFormatString(256, 128, 0xffffff, "Right Stick X : %d", xBuf);
	DrawFormatString(256, 144, 0xffffff, "Right Stick Y : %d", yBuf);
	DrawFormatString(256, 160, 0xffffff, "CameraAngle : %.3f", m_cameraAngleX);

	// デッドゾーン
	if (abs(xBuf) < kStickDeadZone) xBuf = 0;
	if (abs(yBuf) < kStickDeadZone) yBuf = 0;

	// カメラ左右回転
	if (xBuf != 0)
	{
		m_cameraAngleX += xBuf * kCameraRotSpeed;
	}

	// 角度を-π～πに収める
	if (m_cameraAngleX > DX_PI_F)  m_cameraAngleX -= DX_TWO_PI_F;
	if (m_cameraAngleX < -DX_PI_F) m_cameraAngleX += DX_TWO_PI_F;

	// プレイヤー追従先
	Vector3 cameraTarget = m_pPlayer->getCameraTarget();

	// カメラの位置
	Vector3 offset;
	offset.x = sinf(m_cameraAngleX) * kCameraDistance;
	offset.y = kCameraHeight;
	offset.z = cosf(m_cameraAngleX) * kCameraDistance;

	// プレイヤーの後ろから見るようにするなら「-offset」
	Vector3 idealCameraPos = cameraTarget - offset;

	//追従
	m_cameraPos = m_cameraPos + (idealCameraPos - m_cameraPos) * kCameraFollow;
	m_cameraTarget = m_cameraTarget + (cameraTarget - m_cameraTarget) * kCameraFollow;

	// カメラ反映
	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(),m_cameraTarget.ToDxLibVector());
}
