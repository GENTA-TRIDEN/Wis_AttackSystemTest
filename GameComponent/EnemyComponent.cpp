#include "pch.h"
#include "EnemyComponent.h"
// 作成したステートマシンと初期ステートをインクルード
#include "WISLib/GameObject/Component/StateMachineComponent.h"
#include "GameComponent/EnemyState/EnemySearchState.h" 

#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h"
#include "GameComponent/PlayerComponent.h"

using namespace WIS;
using namespace DirectX;
using namespace SimpleMath;

WIS::EnemyComponent::EnemyComponent(Vector3 point1, Vector3 point2)
{
    m_patrolPoints.push_back(point1);
    m_patrolPoints.push_back(point2);
}

WIS::EnemyComponent::~EnemyComponent()
{
}

void WIS::EnemyComponent::OnStart(UserResources* userResources)
{
    // 初期化
    m_moveSpeed = 2.0f;
    m_currentPatrolPointIndex = 0;

    auto device = userResources->GetDeviceResources()->GetD3DDevice();

    // BasicEffect の作成
    m_basicEffect = std::make_unique<BasicEffect>(device);
    m_basicEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        m_inputLayout.ReleaseAndGetAddressOf()
    );

    SelectNextIndex();

    // --- ステートマシンの初期化 ---
    // 自分のGameObjectに StateMachineComponent<EnemyComponent> を追加
    auto stateMachine = GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();

    // 初期ステートとして SearchState をセット
    stateMachine->ChangeState(std::make_shared<EnemySearchState>());
}

void WIS::EnemyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    // OnUpdateのメインロジックは削除されました
    // ロジックは全て StateMachineComponent -> 各State -> Execute... 関数で行われます
}

// アクション ---

void WIS::EnemyComponent::ExecutePatrol()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();
    if (!transform || !rigidBody) return;

    Vector3 NowPos = transform->GetPosition();
    Vector3 nextVelocity = m_moveDirection * m_moveSpeed;

    // Y軸（重力）は今の速度を維持
    Vector3 currentVelocity = rigidBody->GetVelocity();
    nextVelocity.y = currentVelocity.y;

    rigidBody->ChangeVelocity(nextVelocity);

    // 距離チェック (簡易的に0.5f)
    //Y軸は考慮しない
	Vector2 NowPos2D = Vector2(NowPos.x, NowPos.z);
	Vector2 TargetPos2D = Vector2(m_patrolPoints[m_currentPatrolPointIndex].x, m_patrolPoints[m_currentPatrolPointIndex].z);
	if (Vector2::Distance(NowPos2D, TargetPos2D) <= 0.5f) {
		SelectNextIndex();
		m_isCanAttack = true; // 次の攻撃のためにフラグを戻す
	}



    //if (Vector3::Distance(NowPos, m_patrolPoints[m_currentPatrolPointIndex]) <= 0.5f) {
    //    SelectNextIndex();
    //    m_isCanAttack = true; // 次の攻撃のためにフラグを戻す
    //}
}

void WIS::EnemyComponent::ExecuteChase()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();
    if (!transform || !rigidBody) return;

    Vector3 NowPos = transform->GetPosition();
    Vector3 TargetPos = m_PlayerPos;

    Vector3 MoveVec = TargetPos - NowPos;
    MoveVec.y = 0; // 水平のみ

    if (MoveVec.LengthSquared() > 0.0001f) {
        MoveVec.Normalize();
    }

    // 移動
    Vector3 currentVelocity = rigidBody->GetVelocity();
    Vector3 nextVelocity = MoveVec * m_moveSpeed;

    nextVelocity.y = currentVelocity.y;

    rigidBody->ChangeVelocity(nextVelocity);

    // プレイヤーの方向を向く
    XMMATRIX rot = XMMatrixLookAtLH(NowPos, TargetPos, Vector3::Up);
    transform->SetRotation(Quaternion::CreateFromRotationMatrix(rot));
}

void WIS::EnemyComponent::StopMovement()
{
    auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();
    if (rigidBody) {
        //Vector3 vel = rigidBody->GetVelocity();
        //// 水平速度を0に、重力(Y)は維持
        //rigidBody->ChangeVelocity(Vector3(0, vel.y, 0));
    }
}

void WIS::EnemyComponent::SelectNextIndex()
{
    if (m_patrolPoints.empty()) return;

    m_currentPatrolPointIndex++;
    if (m_currentPatrolPointIndex >= m_patrolPoints.size()) {
        m_currentPatrolPointIndex = 0;
    }

    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return;

    Vector3 NowPos = transform->GetPosition();
    Vector3 TargetPos = m_patrolPoints[m_currentPatrolPointIndex];

    // 方向ベクトルを計算
    Vector3 MoveVec = TargetPos - NowPos;
    MoveVec.y = 0;

    // ベクトルの長さが十分ある場合のみ更新
    if (MoveVec.LengthSquared() > 0.0001f) {
        MoveVec.Normalize();
        m_moveDirection = MoveVec;

        Matrix world = Matrix::CreateWorld(NowPos, MoveVec, Vector3::Up);

        // 作成したワールド行列から回転(クォータニオン)を抽出してセット
        transform->SetRotation(Quaternion::CreateFromRotationMatrix(world));
    }
}
bool WIS::EnemyComponent::TryAttackHit(std::shared_ptr<WIS::GameObject> target)
{
    auto playerComp = target->GetComponent<PlayerComponent>();
    if (playerComp) {
        auto charaComp = target->GetComponent<CharacterComponent>();
        if (charaComp && m_isCanAttack) {
            charaComp->DecreaseHP((int)m_AttackDamage);
            m_isCanAttack = false; // 連続ヒット防止

            // 攻撃後に即次のパトロール地点を向くなら
            SelectNextIndex();
            return true;
        }
    }
    return false;
}

void WIS::EnemyComponent::ReceiveDamage()
{

    auto transform = GetOwner()->GetComponent<TransformComponent>();
    auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();
    if (!transform || !rigidBody) return;

    //EnemyComponentが持っているStateMachineを取得



}

// --- 判定メソッド ---

bool WIS::EnemyComponent::CheckPlayerFound()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return false;

    // 攻撃可能かつ発見距離内なら
    return (m_isCanAttack && Vector3::Distance(transform->GetPosition(), m_PlayerPos) <= m_PlayerToFindLength);
}

bool WIS::EnemyComponent::CheckAttackRange()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return false;

    return (Vector3::Distance(transform->GetPosition(), m_PlayerPos) <= m_PlayerAttackLength);
}

bool WIS::EnemyComponent::CheckPlayerLost()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return false;

    return (Vector3::Distance(transform->GetPosition(), m_PlayerPos) >= m_PlayerToFindLength);
}

bool WIS::EnemyComponent::CheckIsMove()
{
    auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();
    if (!rigidBody) return true;



    //MoveLength以下の場合false

    if (m_EnemyIsMoveLength >= rigidBody->GetVelocity().Length());
    return false;


    return true;

}

bool WIS::EnemyComponent::CheckDamageCoolTime()
{
    auto StateMachine = GetOwner()->GetComponent<StateMachineComponent<EnemyComponent>>();
    if (!StateMachine) return false;


    return StateMachine->GetCurrentStateTimer() >= m_TakingDamageCoolTime;

}

void WIS::EnemyComponent::OnRender(UserResources* userResources, const Matrix& view, const Matrix& proj)
{
    // デバッグ描画 (元のコードのまま)
    if (!m_basicEffect) return;

    auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
    auto primitiveBatch = userResources->GetPrimitiveBatch();

    m_basicEffect->SetWorld(Matrix::Identity);
    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(proj);
    m_basicEffect->Apply(context);
    context->IASetInputLayout(m_inputLayout.Get());

    primitiveBatch->Begin();
    
    // 巡回ポイント
    for (Vector3 pos : m_patrolPoints) {
        DX::Draw(primitiveBatch, BoundingSphere(pos, 0.2f), Colors::Black);
    }

    // 索敵範囲
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (transform) {
        DX::Draw(primitiveBatch, BoundingSphere(transform->GetPosition(), m_PlayerToFindLength), Colors::DarkBlue);
    }

    primitiveBatch->End();
}
