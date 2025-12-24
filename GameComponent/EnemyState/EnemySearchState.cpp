#include "pch.h"
#include "EnemySearchState.h"
#include "EnemyChaseState.h" // ŽŸ‚Ì‘JˆÚæ
#include "GameComponent/EnemyComponent.h" // ‘€ì‘ÎÛ
#include "WISLib/GameObject/Component/StateMachineComponent.h"

using namespace WIS;

void EnemySearchState::OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res)
{
    // „‰ñˆÚ“®‚ðŽÀs
    owner->ExecutePatrol();

    // ‘JˆÚ”»’è: UŒ‚‰Â”\‚È‹——£‚ÅƒvƒŒƒCƒ„[‚ð”­Œ©‚µ‚½‚ç Chase ‚Ö
    if (owner->CheckPlayerFound()) {
        auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
        if (sm) {
            sm->ChangeState(std::make_shared<EnemyChaseState>());
        }
    }
}