#pragma once
#include "Scene.h"


class ResultScene :public Scene
{
public:
	ResultScene(SceneController& controller);
	~ResultScene();
	void Update(Input& input)override;
	void Draw()override;

	bool IsFinished() const { return m_finished; }
private:
	//経過フレーム
	int m_frame;
	//フォント
	int m_fontHandle;
	//スコア表示の足されていく演出用
	int m_displayScore;
	int m_finalScore;
	//アニメーションを用
	float m_scoreAnimTime;
	//シーン終了フラグ
	bool m_finished;

	// メンバ関数ポインタを使用して内部状態を書き換える
	// update関数とdraw関数を、現在のTitleSceneの状態に合わせて
	// 切り替えていく

	//フェードインupdate
	void FadeInUpdate(Input& input);
	//通常update
	void NormalUpdate(Input&);
	//フェードアウトupdate
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(ResultScene::*)(Input&);
	//Updateメンバ関数を代入
	UpdateFunc_t m_update;

	void NormalDraw();	// 通常Draw
	void FadeDraw();	// フェードDraw
	using DrawFunc_t = void(ResultScene::*)();
	DrawFunc_t m_draw;	// Drawメンバ関数を代入できるメンバ関数ポインタ


};

