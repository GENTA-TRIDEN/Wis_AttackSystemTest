#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/State/State.h"

#include <memory>
#include <string>

namespace WIS {

    // ステートマシンコンポーネント
    // コンポーネント T を操作するステートを管理します
    template <typename T>
    class StateMachineComponent : public ComponentBase
    {
    private:
        // 現在のステート
        std::shared_ptr<State<T>> m_currentState;

        // 操作対象のコンポーネント (キャッシュ)
        std::shared_ptr<T> m_targetComponent;


        //現在のステートに入ってどれだけ経ったか
        float m_StateTimer;


    public:
        StateMachineComponent() : m_currentState(nullptr), m_targetComponent(nullptr) {}
        virtual ~StateMachineComponent() = default;



        // 開始時にターゲットコンポーネントを取得
        void OnStart(UserResources* userResources) override
        {
            if (auto owner = GetOwner()) {
                m_targetComponent = owner->template GetComponent<T>();
            }
        }

        // 更新処理
        void OnUpdate(const float elapsedTime, UserResources* userResources) override
        {
            if (m_currentState && m_targetComponent) {
                m_StateTimer += elapsedTime;
                // 生ポインタで渡してアクセス負荷を下げる
                m_currentState->OnUpdate(m_targetComponent.get(), elapsedTime, userResources);
            }
        }

        // 衝突イベント
        void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override
        {
            if (m_currentState && m_targetComponent) {
                m_currentState->OnCollision(m_targetComponent.get(), otherObject);
            }
        }

        // ステート変更
        void ChangeState(std::shared_ptr<State<T>> newState)
        {
            if (!m_targetComponent) return;

            // 以前のステート終了
            if (m_currentState) {
                m_currentState->OnExit(m_targetComponent.get());
            }

            // ステート切り替え
            m_currentState = newState;

            // 新しいステート開始
            if (m_currentState) {
                m_StateTimer = 0.0f;
                m_currentState->OnEnter(m_targetComponent.get());
            }
        }

        // 現在のステートポインタを取得 (デバッグ用など)
        std::shared_ptr<State<T>> GetCurrentState() const {
            return m_currentState;
        }

        //現在のステートの経過時間
        float GetCurrentStateTimer() {
            return m_StateTimer;
        }
    };
}