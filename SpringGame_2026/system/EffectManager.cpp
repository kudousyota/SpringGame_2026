#include "EffectManager.h"
#include "EffekseerForDXLib.h"

namespace
{
    // デフォルトスケール
    constexpr float kDefaultScale = 10.0f;
}

EffectManager::EffectManager()
{
    // 初期化
    for (int i = 0; i < EffectType::Max; ++i)
    {
        m_effects[i].handle = -1;
        m_effects[i].scale = kDefaultScale;
        m_effects[i].playInterval = 0;
        m_effects[i].timer = 0;
    }
}

EffectManager::~EffectManager()
{
    // 再生中エフェクト停止
    for (int handle : m_pEffects)
    {
        if (handle != -1)
        {
            StopEffekseer3DEffect(handle);
        }
    }
    m_pEffects.clear();

    // リソース解放
    for (int i = 0; i < EffectType::Max; ++i)
    {
        if (m_effects[i].handle != -1)
        {
            DeleteEffekseerEffect(m_effects[i].handle);
            m_effects[i].handle = -1;
        }
    }
}

void EffectManager::Init()
{
    // Hit
    m_effects[EffectType::Hit].handle = LoadEffekseerEffect("data/Hit2.efk", 3.0f);
    m_effects[EffectType::Hit].scale = 8.0f;
    m_effects[EffectType::Hit].playInterval = 10;

    // Just
    m_effects[EffectType::Just].handle = LoadEffekseerEffect("data/Just.efk", 3.0f);
    m_effects[EffectType::Just].scale = 9.0f;
    m_effects[EffectType::Just].playInterval = 10;
}

void EffectManager::Update()
{
    Effekseer_Sync3DSetting();

    // タイマー更新
    for (int i = 0; i < EffectType::Max; ++i)
    {
        if (m_effects[i].timer > 0)
        {
            m_effects[i].timer--;
        }
    }

    // 再生中エフェクト管理
    for (auto it = m_pEffects.begin(); it != m_pEffects.end(); )
    {
        int handle = *it;

        if (handle == -1 || !IsEffekseer3DEffectPlaying(handle))
        {
            it = m_pEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    UpdateEffekseer3D();
}

void EffectManager::Draw()
{
    DrawEffekseer3D();
}

void EffectManager::PlayEffect(int type, const Vector3& pos)
{
    // 範囲チェック
    if (type < 0 || type >= EffectType::Max) return;

    EffectData& effect = m_effects[type];

    if (effect.handle == -1) return;
    if (effect.timer > 0) return;

    int playing = PlayEffekseer3DEffect(effect.handle);
    if (playing == -1) return;

    m_pEffects.push_back(playing);

    // 位置
    SetPosPlayingEffekseer3DEffect(playing, pos.x, pos.y, pos.z);

    // スケール
    SetScalePlayingEffekseer3DEffect(
        playing,
        effect.scale,
        effect.scale,
        effect.scale
    );

    // クールタイム
    effect.timer = effect.playInterval;
}