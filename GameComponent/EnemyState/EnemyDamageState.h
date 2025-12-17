#pragma once
#include "WISLib/GameObject/State/State.h"

namespace WIS {
    class EnemyComponent; // ‘O•ûéŒ¾

    class EnemyDamageState : public State<EnemyComponent>
    {
    public:
        void OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res) override;
    };
}