#include "pch.h"
#include "EnemyDamageState.h"
#include "EnemySearchState.h" // 次の遷移先
#include "GameComponent/EnemyComponent.h" // 操作対象
#include "WISLib/GameObject/Component/StateMachineComponent.h"

using namespace WIS;

void EnemyDamageState::OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res)
{
    // 1.ダメージを受けた際の処理を実行
    owner->ReceiveDamage();


    //クールタイムが終わったらSearchへ変更
    if (owner->CheckDamageCoolTime()) {

        auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
        if (sm) {
            sm->ChangeState(std::make_shared<EnemySearchState>());
        }
    }
}