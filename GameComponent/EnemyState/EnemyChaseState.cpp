#include "pch.h"
#include "EnemyChaseState.h"
#include "EnemyAttackState.h" // 攻撃へ
#include "EnemySearchState.h" // 見失ったら戻る
#include "GameComponent/EnemyComponent.h" // 操作対象
#include "WISLib/GameObject/Component/StateMachineComponent.h"

using namespace WIS;

void EnemyChaseState::OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res)
{
    // 1. 追跡移動を実行
    owner->ExecuteChase();

    auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
    if (!sm) return;

    // 2. 遷移判定: 攻撃範囲内なら Attack へ
    if (owner->CheckAttackRange()) {
        sm->ChangeState(std::make_shared<EnemyAttackState>());
    }
    // 遷移判定: プレイヤーを見失ったら Search へ
    else if (owner->CheckPlayerLost()) {
        // 次のパトロール地点を決めてから戻る
        owner->SelectNextIndex();
        sm->ChangeState(std::make_shared<EnemySearchState>());
    }
}