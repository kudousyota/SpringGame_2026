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

    // 汎用エフェクト再生
    void PlayEffect(int type, const Vector3& pos);

    enum EffectType
    {
        Hit,
        Just,
        Max
    };
private:
   

    struct EffectData
    {
        int handle = -1;
        float scale = 1.0f;
        int playInterval = 0;
        int timer = 0;
    };

    // エフェクト情報まとめて管理
    EffectData m_effects[EffectType::Max];

    // 再生中の実体
    std::vector<int> m_pEffects;
};

