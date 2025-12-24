#include "pch.h"
#include "EnemyDamageState.h"
#include "EnemySearchState.h" // ŽŸ‚Ì‘JˆÚæ
#include "GameComponent/EnemyComponent.h" // ‘€ì‘ÎÛ
#include "WISLib/GameObject/Component/StateMachineComponent.h"

using namespace WIS;

void WIS::EnemyDamageState::OnEnter(EnemyComponent* owner)
{

}

void EnemyDamageState::OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res)
{

    owner->ReceiveDamage();

    if (owner->CheckDamageCoolTime()) {

        auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
        if (sm) {
            owner->SelectNextIndex();
            sm->ChangeState(std::make_shared<EnemySearchState>());
        }
    }
}