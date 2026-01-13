#include "pch.h"
#include "InstantAttackComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"

using namespace WIS;

void InstantAttackComponent::OnStart(UserResources* userResources)
{
    // オーナーのCollisionComponentを取得
    auto owner = GetOwner();
    if (!owner) return;

    auto collisionComp = owner->GetComponent<CollisionComponent>();
    if (collisionComp)
    {
        // 衝突時のコールバックを登録
        // std::bindを使って、メンバ関数 OnHit を呼び出すように設定
        collisionComp->SetOnCollisionCallback(
            [this](std::shared_ptr<GameObject> other) {
                this->OnHit(other);
            }
        );
    }
}

void InstantAttackComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    // 寿命管理
    m_CurrentTime += elapsedTime;
    if (m_CurrentTime >= m_LifeTime)
    {
        // 寿命が尽きたら非アクティブ化 (CollisionManager等の対象外にする)
        if (auto owner = GetOwner()) {
            owner->SetActive(false);
            // シーンの仕様に合わせて、ここで完全に削除(OnRelease)を呼ぶか、
            // シーン管理側でActiveフラグを見て削除するか連携してください
        }
    }
}

void InstantAttackComponent::OnHit(std::shared_ptr<GameObject> other)
{
    // 1. 多重ヒットチェック
    // 既にヒストリーリストにある相手なら何もしない
    GameObject* targetPtr = other.get();
    auto it = std::find(m_HitHistory.begin(), m_HitHistory.end(), targetPtr);
    
    if (it != m_HitHistory.end()) {
        return; // 既に攻撃済み
    }

    auto chara = other->GetComponent<WIS::CharacterComponent>();
    if (!chara)return;


	other->GetComponent<WIS::CharacterComponent>()->TakeAttack(m_Damage);

    // 3. 履歴に追加 (このフレーム以降は無視される)
    m_HitHistory.push_back(targetPtr);
}