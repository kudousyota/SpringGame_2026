#pragma once
class Effect
{
public:
	Effect();
	~Effect();
	void Init();
	void Update();
	void Draw();
private:
	int m_effectHandle;
};

