#include "Player.h"
#include "Dxlib.h"
#include "Input.h"

namespace
{
	constexpr float speed = 3.0f;
}

Player::Player() :
	m_modelHandle(-1),
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
	if (input.IsPressed("up"))
	{
		m_pos.z += speed;
	}
	if (input.IsPressed("down"))
	{
		m_pos.z -= speed;
	}
	if (input.IsPressed("left"))
	{
		m_pos.x -= speed;
	}
	if (input.IsPressed("right"))
	{
		m_pos.x += speed;
	}
	
	MV1SetPosition(m_modelHandle,m_pos.ToDxLibVector());

}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}
