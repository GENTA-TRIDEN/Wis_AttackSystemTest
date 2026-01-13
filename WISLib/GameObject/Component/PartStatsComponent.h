#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <memory>

#include "WISLib/UserInterface/BombEffect.h"


namespace WIS {
    class GameObject; // 前方宣言
    class CharacterComponent; // ApplyOnHitEffectのために必要

    class PartStatsComponent : public WIS::ComponentBase
    {
    private:
        // パーツ固有の静的ステータス補正値
        float m_partWeight = 0.5f;        // このパーツがもたらす追加の重さ (WeaponComponentが計算に利用)
        float m_centerOfMassValue = 0.8f; // このパーツの重心位置（0.1:手元 ～ 1.0:先端） (WeaponComponentが計算に利用)
        int m_damageBonus = 5;            // このパーツがもたらす攻撃力ボーナス (WeaponComponentが計算に利用)

		WIS::ItemKinds m_partType = WIS::ItemKinds::GEAR;

		//BombEffect
		std::unique_ptr<WIS::BombEffect> m_Bombeffect;


    public:
		PartStatsComponent(WIS::ItemKinds partType) :m_partType(partType) {}
        // ComponentBaseの純粋仮想関数を実装するためデストラクタは必要
        ~PartStatsComponent() override = default;

        // 外部からステータスを取得するためのGetter
        float GetPartWeight() const { return m_partWeight; }
        float GetCenterOfMassValue() const { return m_centerOfMassValue; }
        int GetDamageBonus() const { return m_damageBonus; }

        // ComponentBaseのオーバーライド
        void OnStart(UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
		void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
        void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override {}

        // 衝突時にパーツ固有の特殊効果を適用する (WeaponComponent::OnCollision から呼び出される)
        void ApplyOnHitEffect(std::shared_ptr<WIS::GameObject> hitTarget, float baseDamage,DirectX::SimpleMath::Vector3 effectPos);
    };
}