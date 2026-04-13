#include "Effect.h"
#include "EffekseerForDXLib.h"

Effect::Effect():
	m_pos({ 0.0f,0.0f,0.0f }),
	m_effectHandle(-1)
{
}

Effect::~Effect()
{
}

void Effect::Init()
{
	m_effectHandle = LoadEffekseerEffect("data/Hit.efk");
}

void Effect::Update()
{
}

void Effect::Draw()
{
}
