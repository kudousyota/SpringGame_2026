#include "Camera.h"
#include "DxLib.h"
#include "Player.h"

namespace 
{
	//注視点からカメラに向かうベクトル
	const Vector3 kTargetToCamera = VGet(0.0f, 250.0f, 1100.0f);

	constexpr float kCameraFollow = 0.1f;
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	Vector3 cameraTarget = m_pPlayer->getCameraTarget();
	float playerAng = m_pPlayer->getAngle();

	Vector3 targetToCamera = kTargetToCamera.RotateY(playerAng);

	m_cameraTarget = cameraTarget;
	m_cameraPos = cameraTarget + targetToCamera;

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(), m_cameraTarget.ToDxLibVector());
}

void Camera::End()
{
}

void Camera::Update()
{
	// カメラの注視点
	Vector3 cameraTarget = m_pPlayer->getCameraTarget();
	float playerAng = m_pPlayer->getAngle();

	// プレイヤーの向きに応じてカメラ位置オフセットを回転
	Vector3 targetToCamera = kTargetToCamera.RotateY(playerAng);

	// 目標カメラ位置
	Vector3 desiredCameraPos = cameraTarget + targetToCamera;

	// カメラ位置をなめらかに追従
	m_cameraPos += (desiredCameraPos - m_cameraPos) * kCameraFollow;

	// 注視点をなめらかに追従
	m_cameraTarget += (cameraTarget - m_cameraTarget) * kCameraFollow;

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(), m_cameraTarget.ToDxLibVector());
}
