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
    //タイマーがどのくらいか分かる関数
	void SetTimer(int timer);

	// 残りフレーム数を取得
	int GetRemainingFrames() const { return m_gameTimer; }

	
	static int GetInitialFrames();

	
private:
	int m_gameTimer;
};

