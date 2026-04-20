#pragma once
#include "Vector3.h"
#include <vector>

class EffectManager
{

public:
    EffectManager();
    ~EffectManager();

    void Init();
    void Update();
    void Draw();

    // ヒットエフェクト再生
    void PlayHitEffect(const Vector3& pos);

private:
    enum EffectType
    {
        //ヒット
        Hit,
        //ジャスト
        Just,
        //配列サイズ
        Max
    };

    // Effekseerエフェクトのリソースハンドル
    int m_effectHandles[EffectType::Max];

    // 再生中エフェクトのハンドル群(実体を複数管理)
    std::vector<int> m_pEffects;

    // 再生間隔用タイマー
    int m_playTimer;
};

