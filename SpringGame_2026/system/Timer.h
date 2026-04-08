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

	// 初期のフレーム数（秒数 * FPS）を取得
	static constexpr int kFps = 60;
	static constexpr int kTimeLimit = 60; // 秒
	static int GetInitialFrames() { return kTimeLimit * kFps; }

	
private:
	int m_gameTimer;
};

