#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include "WISLib/Camera/CameraManager.h"

namespace WIS {
    class EnemyComponent : public WIS::ComponentBase
    {

    private:
        

        //移動方向
        DirectX::SimpleMath::Vector3 m_moveDirection;


        //移動速度をプレイヤーより遅め
        float m_moveSpeed = 3.0f; // 移動速度

        std::vector< DirectX::SimpleMath::Vector3> m_patrolPoints;

        int m_currentPatrolPointIndex;

        //仮で雑なState作る
        //@TODO:StateMachineを導入する
        //->サンドブレイカーのステートマシーン参照


        //ステート
        enum class state{
            Search = 0,//プレイヤーを探している
            Chase=1,//プレイヤーを追いかける
            Attack=2//プレイヤーを攻撃する
        };
        //現在のステート
        state m_currentState;


        // デバッグ描画用のBasicEffect
        std::unique_ptr<DirectX::BasicEffect> m_basicEffect = nullptr;

        //入力レイアウト
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

        DirectX::SimpleMath::Vector3 m_PlayerPos;

        float m_PlayerToFindLength = 5.0f;

        float m_PlayerAttackLength = 0.8;

        bool m_isCanAttack;

        float m_AttackLength = 5.0f;

    public:
        EnemyComponent(DirectX::SimpleMath::Vector3 point1, DirectX::SimpleMath::Vector3 point2);
        ~EnemyComponent();

        void OnStart(UserResources* userResources) override;
        void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj) override;
        void OnRelease() override;

        //子リジョンに当たった時に攻撃する
        void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;


        //移動先のぽいんとに到着したらこれを呼び出し次の場所へいく
        void SelectNextIndex();

        void SetPlayerPos(DirectX::SimpleMath::Vector3 pos) {
            m_PlayerPos = pos;
        }




    };
}

