#include "Enemy.h"
#include "Player.h"
#include "../system/Timer.h"
#include "Dxlib.h"
#include <cmath>
#include "../system/Effect.h"
#include "../system/EffectManager.h"
namespace
{
	//弾の速度
	constexpr float kBulletSpeed = 15.0f;

	//アニメーションの名前
	const char* const kIdleAnimName = "MonsterArmature|Idle";
	const char* const kBiteAnimName = "MonsterArmature|Bite_Front";

	// 弾の半径(描画時と同じ値)
	constexpr float kBulletRadius = 10.0f;

	//弾のサイズ
	constexpr float kBulletSize = 50.0f;

	// 敵が弾を発射してから消えるまでの最大距離
	constexpr float kBulletMaxDistance = 1500.0f;

	// 攻撃頻度の初期値(フレーム数)
	constexpr int kDefaultAttackCoolTime = 60; // 1秒
}

void Enemy::SetPos(const Vector3& pos)
{
	m_pos = pos;
	if (m_modelHandle != -1)
	{
		MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());
		// Update matrix as well to keep correct rotation/translation
		MATRIX rotMtx = MGetRotY(m_angle);
		MATRIX transMtx = MGetTranslate(m_pos.ToDxLibVector());
		MATRIX world = MMult(rotMtx, transMtx);
		MV1SetMatrix(m_modelHandle, world);
	}
}

Enemy::Enemy() :
	m_modelHandle(-1),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletPos(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletDir(VGet(0.0f, 0.0f, 0.0f)),
	m_bulletStartPos(VGet(0.0f, 0.0f, 0.0f)),
	m_hasBullet(false),
	m_shotTimer(0),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_lastAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_animChangeFrame(0),
	m_angle(0.0f),
	m_BiteAnimeFrame(0),
	m_bulletSpeed(0.0f),
	m_attackCoolTime(0),
	m_isAttac(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	m_modelHandle = MV1LoadModel("data/Enemy.mv1");

	// 敵の初期位置を設定
	m_pos = VGet(0.0f, 350.0f, 350.0f);
	MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());

	int animNo = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
	m_currentAnimCount = 0.0f;

	m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
	m_currentAnimIndex = animNo;

	m_lastAnimHandle = -1;
	m_lastAnimCount = 0.0f;

	m_animChangeFrame = 0;

	// デフォルト値の設定
	if (m_bulletSpeed <= 0.0f)
	{
		m_bulletSpeed = kBulletSpeed;
	}
	// デフォルトの攻撃頻度を設定
	if (m_attackCoolTime <= 0)
	{
		m_attackCoolTime = kDefaultAttackCoolTime; // デフォルト1秒
	}
}

void Enemy::End()
{
	if (m_modelHandle != -1)
	{
		if (m_cureentAnimHandle != -1)
		{
			MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
			m_cureentAnimHandle = -1;
		}
		if (m_lastAnimHandle != -1)
		{
			MV1DetachAnim(m_modelHandle, m_lastAnimHandle);
			m_lastAnimHandle = -1;
		}
		MV1DeleteModel(m_modelHandle);
		m_modelHandle = -1;
	}
}

void Enemy::Update(Player& player, const Timer& timer, EffectManager& effectManager)
{
	// プレイヤーの位置を取得
	Vector3 playerPos = player.GetPos();

	// 敵の向きをプレイヤーに向ける
	Vector3 toPlayer = playerPos - m_pos;

	//Y軸は回転させない
	toPlayer.y = 0.0f;

	//回転
	if (toPlayer.SqMagnitude() > 0.0001f)
	{
		// 敵の向きをプレイヤーに向ける
		toPlayer = toPlayer.Normalize();

		m_angle = atan2f(toPlayer.x, toPlayer.z) + DX_PI_F;

		// モデル行列を毎フレーム更新して回転と位置を反映する
		MATRIX rotMtx = MGetRotY(m_angle);
		MATRIX transMtx = MGetTranslate(m_pos.ToDxLibVector());
		MATRIX world = MMult(rotMtx, transMtx);
		MV1SetMatrix(m_modelHandle, world);
	}

    // クールタイムを更新
	m_shotTimer++;

	// 残り時間に応じて攻撃頻度をスケールする
	// Timerの残りフレーム数を取得
	int remaining = timer.GetRemainingFrames();
	int initial = Timer::GetInitialFrames();
	// 残り時間が少ないほど攻撃間隔を短くする
	int scaledCool = m_attackCoolTime;
	if (initial > 0)
	{
		//時間が減るほどscaledCoolが小さくなるようにする。残り時間が初期の半分のときに攻撃間隔が75%になるように
		//timerは0.25f ~ 1.0fの範囲になるようにする
		//例えば残り時間が初期の半分のとき、timerは0.625fになる。残り時間が初期の25%のとき、timerは0.4375fになる。
		float timeRatio = static_cast<float>(remaining) / static_cast<float>(initial);

		if (timeRatio < 0.0f) timeRatio = 0.0f;
		if (timeRatio > 1.0f) timeRatio = 1.0f;

		// 時間が減るほど攻撃間隔を短くする
		float multiplier = 0.25f + 0.75f * timeRatio;

		scaledCool = static_cast<int>(m_attackCoolTime * multiplier);
		if (scaledCool < 1) scaledCool = 1;
	}

	// 一定時間たったら発射
	if (!m_hasBullet && !m_isAttac && m_shotTimer >= scaledCool)
	{
		// 弾を敵の前方に少しオフセットしてスポーンし、方向ベクトルがゼロにならないようにする
		m_bulletDir = playerPos - m_pos;
		// Y軸方向の差は弾を水平に飛ばすため無視
		m_bulletDir.y = 0.0f;
		// 方向ベクトルがほとんどゼロなら前方ベクトルを代入
		if (m_bulletDir.SqMagnitude() <= 0.000001f)
		{
			m_bulletDir = VGet(0.0f, 0.0f, 1.0f);
		}
		else
		{
			m_bulletDir = m_bulletDir.Normalize();
		}

		// 弾を敵の中心ではなく少し前方に出す
		constexpr float kSpawnOffset = kBulletRadius + 30.0f;
		m_bulletPos = m_pos + (m_bulletDir * kSpawnOffset);
		m_bulletStartPos = m_bulletPos;
		m_hasBullet = true;
		m_shotTimer = 0;

		m_isAttac = true;

		// 攻撃アニメーションを開始
		// Biteアニメに切り替え
		int animNo = MV1GetAnimIndex(m_modelHandle, kBiteAnimName);
		if (animNo >= 0 && animNo != m_currentAnimIndex)
		{
			// 今のアニメを外す
			if (m_cureentAnimHandle != -1)
			{
				MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
			}

			// Biteアニメをセット
			m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
			m_currentAnimIndex = animNo;
			m_currentAnimCount = 0.0f;
			m_animChangeFrame = 0;
		}
	}

	// 弾を進める
    if (m_hasBullet)
	{
		// 弾の速度を設定値から取得。0以下ならデフォルト値を使用
		float bulletspeed = (m_bulletSpeed > 0.0f) ? m_bulletSpeed : kBulletSpeed;
		m_bulletPos += m_bulletDir * bulletspeed;

		// プレイヤーとの当たり判定
		// プレイヤーの中心と半径を取得
		Vector3 playerCenter = player.GetColCenter();
		float playerRadius = player.GetColRadius();

		float bulletRadius = kBulletRadius;

		// 距離計算
		float distSq = (m_bulletPos - playerCenter).SqMagnitude();
		float justDistSq = distSq;

		// 半径
		float hitDist = playerRadius + bulletRadius;
		float justDist = player.GetJustColRadius() + bulletRadius;

        // ジャスト判定
		// 現在ジャストの範囲ににいるか、
		// 直近30フレーム以内にジャストウィンドウにいた場合にも許可する
		const int kJustRecentFrames = 30;
		bool recentJust = player.IsInJustWindow() || player.WasInJustWindowRecently(kJustRecentFrames);
        if (player.IsDodging() && recentJust &&
			justDistSq <= justDist * justDist)
		{
			player.OnJustDodge();
			effectManager.PlayHitEffect(m_bulletPos);
			m_hasBullet = false;
		}
		// 通常ヒット
        else if (distSq <= hitDist * hitDist)
		{
			if (!player.IsInvincible())
			{
				player.OnHit();
				effectManager.PlayHitEffect(m_bulletPos);
				m_hasBullet = false;
			}
		}

		// 一定距離進んだら消すルート計算を避けるため二乗距離で判定
		constexpr float kMaxDist = kBulletMaxDistance;
		float travelDistSq = (m_bulletPos - m_bulletStartPos).SqMagnitude();
		if (travelDistSq > kMaxDist * kMaxDist)
		{
			m_hasBullet = false;
		}
        // アニメ切り替えは発射時に行う。ここでは継続時間の管理のみ
	}
	// アニメーション
	// アニメ進行
	m_currentAnimCount += 0.5f;
	float currentTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);

	// Bite中はループさせない
	if (m_currentAnimIndex == MV1GetAnimIndex(m_modelHandle, kBiteAnimName))
	{
		if (m_currentAnimCount >= currentTotal)
		{
			m_currentAnimCount = currentTotal;
		}
	}
	else
	{
		// Idleなどはループ
		if (m_currentAnimCount >= currentTotal)
		{
			m_currentAnimCount -= currentTotal;
		}
	}

	MV1SetAttachAnimTime(m_modelHandle, m_cureentAnimHandle, m_currentAnimCount);


	// Biteアニメが最後まで再生されたら Idle に戻す
	if (m_isAttac && m_currentAnimIndex == MV1GetAnimIndex(m_modelHandle, kBiteAnimName))
	{
		float biteTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);

		if (m_currentAnimCount >= biteTotal)
		{
			int idleAnimNo = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
			if (idleAnimNo >= 0 && idleAnimNo != m_currentAnimIndex)
			{
				// 今のBiteアニメを外す
				if (m_cureentAnimHandle != -1)
				{
					MV1DetachAnim(m_modelHandle, m_cureentAnimHandle);
				}

				// Idleアニメをセット
				m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, idleAnimNo, -1, -1);
				m_currentAnimIndex = idleAnimNo;
				m_currentAnimCount = 0.0f;
				m_animChangeFrame = 0;
			}

			m_isAttac = false;
		}
	}
}

void Enemy::Draw()
{
	// 敵のモデルを描画
	MV1DrawModel(m_modelHandle);
	// 弾を描画
	if (m_hasBullet)
	{
		DrawSphere3D(m_bulletPos.ToDxLibVector(), kBulletSize, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
	}
}
