#pragma once
#include "WISLib/GameObject/State/State.h"
namespace WIS {
    class EnemyComponent;

    class EnemyAttackState : public State<EnemyComponent>
    {
    public:
        void OnEnter(EnemyComponent* owner) override;
        void OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res) override;
        void OnCollision(EnemyComponent* owner, std::shared_ptr<WIS::GameObject> otherObject) override;
    };
}