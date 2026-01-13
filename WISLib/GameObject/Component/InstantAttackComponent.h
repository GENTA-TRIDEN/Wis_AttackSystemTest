#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include <vector>
#include <algorithm>

namespace WIS {
    class InstantAttackComponent : public ComponentBase
    {
    public:
        // コンストラクタ (寿命とダメージ量を指定)
        InstantAttackComponent(float lifeTime, int damage)
            : m_LifeTime(lifeTime), m_Damage(damage) {
        }

        ~InstantAttackComponent() = default;

        void OnStart(UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;

    private:
        // コールバック関数: 衝突したときに呼ばれる
        void OnHit(std::shared_ptr<GameObject> other);

    private:
        float m_LifeTime;       // 生存時間（秒）
        float m_CurrentTime = 0.0f;
        int m_Damage;

        // 一度当たった相手を記録するリスト (多重ヒット防止用)
        std::vector<GameObject*> m_HitHistory;
    };
}