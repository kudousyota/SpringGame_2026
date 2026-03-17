#include "Player.h"
#include "Dxlib.h"
Player::Player():
	m_modelHandle(-1)
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

void Player::Update()
{
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}
