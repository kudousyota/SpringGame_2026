#pragma once
#include "../system/Vector3.h"
class Input;
class Player
{
public:
	Player();
	~Player();

	void Init();
	void End();
	void Update(Input input);
	void Draw();

	
	
private:
	enum class Inputdata
	{
		None,
		Up,
		Down,
		Left,
		Right
	};
	int m_modelHandle;

	Vector3 m_pos;

	float m_abgle;

	//2回入力されたか
	bool m_isDoubleInput;
	float m_doubleInputTimer;

	Inputdata m_inputData;
	bool m_isInput;
	
	// 回避中か
	bool m_isDodge;
	// 回避の残り時間
	float m_dodgeTimer;  

};

