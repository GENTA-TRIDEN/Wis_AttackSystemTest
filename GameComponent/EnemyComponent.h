#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include "WISLib/Camera/CameraManager.h"
#include <vector>

namespace WIS {
    class EnemyComponent : public WIS::ComponentBase
    {
    private:
        // 移動方向
        DirectX::SimpleMath::Vector3 m_moveDirection;

        // パラメータ
        float m_moveSpeed = 3.0f;
        std::vector<DirectX::SimpleMath::Vector3> m_patrolPoints;
        int m_currentPatrolPointIndex = 0;

        // プレイヤー情報
        DirectX::SimpleMath::Vector3 m_PlayerPos;
        float m_PlayerToFindLength = 5.0f;  // 発見距離
        float m_PlayerAttackLength = 0.8f;  // 攻撃射程
        float m_AttackDamage = 5.0f;        // 攻撃力
        bool m_isCanAttack = true;          // 攻撃可能フラグ

        //
        float m_EnemyIsMoveLength = 0.01f;  //物理挙動中判定用
        float m_TakingDamageCoolTime = 2.0f;





        // デバッグ描画
        std::unique_ptr<DirectX::BasicEffect> m_basicEffect = nullptr;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    public:
        EnemyComponent(DirectX::SimpleMath::Vector3 point1, DirectX::SimpleMath::Vector3 point2);
        ~EnemyComponent();

        void OnStart(UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

        // 衝突処理は StateMachineComponent に任せるため、ここは空もしくは何もしない
        void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override {}

        // --- ステートから呼び出すアクション (Action Methods) ---

        // 巡回移動を行う
        void ExecutePatrol();

        // 追跡移動を行う
        void ExecuteChase();

        // 移動停止
        void StopMovement();

        // 次のパトロール地点を選択
        void SelectNextIndex();

        // 攻撃ヒット処理 (成功したらtrue)
        bool TryAttackHit(std::shared_ptr<WIS::GameObject> target);

        //攻撃をうける
        void ReceiveDamage();

            

        // --- 判定メソッド (Condition Checks) ---

        bool CheckPlayerFound();
        bool CheckAttackRange();
        bool CheckPlayerLost();
       

        //
        bool CheckIsMove();
        bool CheckDamageCoolTime();

        // --- セッターなど ---

        void SetPlayerPos(DirectX::SimpleMath::Vector3 pos) {
            m_PlayerPos = pos;
        }

        void AddPatrolPoint(DirectX::SimpleMath::Vector3 newPoint) {
            m_patrolPoints.push_back(newPoint);
        }
    };
}