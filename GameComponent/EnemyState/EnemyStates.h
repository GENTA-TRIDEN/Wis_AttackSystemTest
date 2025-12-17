#pragma once
#include "GameComponent/EnemyComponent.h"
#include "WISLib/GameObject/Component/StateMachineComponent.h"

namespace WIS {

    // --- 探索 (巡回) ステート ---
    class EnemySearchState : public State<EnemyComponent>
    {
    public:
        void OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res) override {
            // 1. 巡回移動を実行
            owner->ExecutePatrol();

            // 2. 遷移判定: プレイヤーを見つけたら Chase へ
            if (owner->CheckPlayerFound()) {
                ChangeState(owner, std::make_shared<EnemyChaseState>());
            }
        }

    private:
        // ステート変更用ヘルパー
        void ChangeState(EnemyComponent* owner, std::shared_ptr<State<EnemyComponent>> newState) {
            auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
            if (sm) sm->ChangeState(newState);
        }
    };

    // --- 追跡 ステート ---
    class EnemyChaseState : public State<EnemyComponent>
    {
    public:
        void OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res) override {
            // 1. 追跡移動を実行
            owner->ExecuteChase();

            // 2. 遷移判定: 攻撃範囲なら Attack へ
            if (owner->CheckAttackRange()) {
                ChangeState(owner, std::make_shared<EnemyAttackState>());
            }
            // 3. 遷移判定: 見失ったら Search へ
            else if (owner->CheckPlayerLost()) {
                ChangeState(owner, std::make_shared<EnemySearchState>());
            }
        }

    private:
        void ChangeState(EnemyComponent* owner, std::shared_ptr<State<EnemyComponent>> newState) {
            auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
            if (sm) sm->ChangeState(newState);
        }
    };

    // --- 攻撃 ステート ---
    class EnemyAttackState : public State<EnemyComponent>
    {
    public:
        void OnEnter(EnemyComponent* owner) override {
            // 攻撃開始時は止まる
            owner->StopMovement();
        }

        void OnUpdate(EnemyComponent* owner, float elapsedTime, UserResources* res) override {
            // 攻撃アニメーション待ちや、硬直時間処理などをここに書く
            // 今回はCollisionで判定しているので、ここでは特に何もしないか、
            // 一定時間経過でSearchに戻す処理などを入れる
        }

        // 衝突判定はステート側で受け取る
        void OnCollision(EnemyComponent* owner, std::shared_ptr<WIS::GameObject> otherObject) override {
            // プレイヤーに当たったらダメージを与えて探索に戻る
            if (owner->TryAttackHit(otherObject)) {
                auto sm = owner->GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
                if (sm) sm->ChangeState(std::make_shared<EnemySearchState>());
            }
        }
    };
}