#pragma once
class Timer
{
public:
	Timer();
	~Timer();
	void Init();
	void End();
	void Update();
	void Draw();

	bool IsTimeUp() const;
private:
	int m_gameTimer;
};

