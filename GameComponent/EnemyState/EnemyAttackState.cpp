#include "pch.h"
#include "EnemyAttackState.h"
#include "EnemySearchState.h" // 攻撃終わったら戻る
#include "GameComponent/EnemyComponent.h" // 操作対象
#include "WISLib/GameObject/Component/StateMachineComponent.h"

using namespace WIS;

void EnemyAttackState::OnEnter(EnemyComponent* owner)
{
    // 攻撃開始時は足を止める
    owner->StopMovement();
}

void EnemyAttackState::OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res)
{
    // ここで攻撃アニメーションの待機時間などを管理してもよい
    // 今回はCollisionのみで判定
}

void EnemyAttackState::OnCollision(EnemyComponent* owner, std::shared_ptr<WIS::GameObject> otherObject)
{

    // プレイヤーにダメージを与えようとする
    if (owner->TryAttackHit(otherObject)) {
        // 攻撃が成功したら Search に戻る
        auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
        if (sm) {
            owner->SelectNextIndex();
            sm->ChangeState(std::make_shared<EnemySearchState>());
        }
    }
}