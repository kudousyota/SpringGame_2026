#include "Player.h"
#include "Dxlib.h"
#include "Input.h"

namespace
{
	constexpr float kspeed = 3.0f;
	constexpr float kDoubleInputSpeed = 6.0f;
	//2回入力されたとみなす時間
	constexpr int kDoubleInputTime = 30;
}

Player::Player() :
	m_modelHandle(-1),
	m_abgle(0.0f),
	m_isDoubleInput(false),
	m_doubleInputTimer(0.0f),
	m_pos(VGet(0, 0, 0))

{
}
Player::~Player()
{
	
}

void Player::Init()
{
	m_modelHandle = MV1LoadModel("data/Player.mv1");
}

void Player::End()
{
	MV1DeleteModel(m_modelHandle);
}

void Player::Update(Input input)
{
	//0.5秒以内に同じ入力がされたらダッシュ
	m_doubleInputTimer++;



	if (input.IsPressed("up"))
	{
		m_pos.z += kspeed;
	}
	if (input.IsPressed("down"))
	{
		m_pos.z -= kspeed;
	}
	if (input.IsPressed("left"))
	{
		m_pos.x -= kspeed;
	}
	if (input.IsPressed("right"))
	{
		m_pos.x += kspeed;
	}
	
	
	

	MV1SetPosition(m_modelHandle,m_pos.ToDxLibVector());

}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}

