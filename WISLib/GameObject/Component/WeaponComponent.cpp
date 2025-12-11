#include "pch.h"


#include "WeaponComponent.h"
#include "CharacterComponent.h" 
#include "PartStatsComponent.h"
#include "WISLib/GameObject/GameObject.h" // GetComponentのために必要

#include "UserResources.h"

using namespace WIS;

WeaponComponent::WeaponComponent()
{
    CalculateFinalStats();
}

WeaponComponent::~WeaponComponent() {}

void WeaponComponent::OnStart(UserResources* userResources)
{
    // 初期化時やパーツのアタッチ後に、必ずステータスを集計・計算する
    AggregateAndCalculateStats();
}

void WeaponComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    // 攻撃クールダウンのタイマー処理
    if (!m_canAttack) {
        m_coolDownTimer += elapsedTime;
        if (m_coolDownTimer >= m_attackCoolDown) {
            m_canAttack = true;
            m_coolDownTimer = 0.0f;
        }
    }

    // 武器を振っている最中の処理 (ヒット判定期間の管理)
    if (m_isSwinging) {
        m_swingTimer += elapsedTime;
        // 振りの期間が終了したらヒット判定を無効にする
        if (m_swingTimer >= m_swingDuration) {
            m_isSwinging = false;
            m_swingTimer = 0.0f;
            m_hasDealtDamageThisSwing = false; // 次の振りのためにリセット
        }
    }
}

void WIS::WeaponComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    auto font = userResources->GetDebugFont();



    font->AddString((L"currentAddedWeight:" + std::to_wstring(m_currentAddedWeight)).c_str(), { 500.0f, 20.0f });
    font->AddString((L"attackPower:" + std::to_wstring(m_attackPower)).c_str(), { 500.0f, 50.0f });
    font->AddString((L"currentCenterOfMass:" + std::to_wstring(m_currentCenterOfMass)).c_str(), { 500.0f, 80.0f });

}

void WeaponComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
    // 武器が振られている最中であり、かつ、まだ今回の振りでダメージを与えていないかチェック
    if (m_isSwinging && !m_hasDealtDamageThisSwing)
    {
        // 衝突相手からCharacterComponentを取得
        auto TargetCharacter = otherObject->GetComponent<WIS::CharacterComponent>();

        if (TargetCharacter && TargetCharacter->GetObjectType() == WIS::ObjectType::ENEMY)
        {
            // --- 1. 武器本体のダメージ処理 ---
            TargetCharacter->DecreaseHP(m_attackPower);
            m_hasDealtDamageThisSwing = true; // ダメージを与えたのでフラグを立てる


            // --- 2. 各パーツの衝突時ロジックを実行 ---
            for (auto& weakPart : m_AttachedParts) {
                if (auto partObject = weakPart.lock()) {
                    // PartStatsComponentを取得し、固有のヒット時効果を適用
                    if (auto statsComp = partObject->GetComponent<WIS::PartStatsComponent>()) {
                        // ターゲットオブジェクトを渡して、パーツ固有の処理を実行させる
                        statsComp->ApplyOnHitEffect(otherObject,m_baseDamage);
                    }
                }
            }
        }
    }
}

// パーツオブジェクトをリストに追加
void WeaponComponent::AttachPart(std::shared_ptr<WIS::GameObject> partObject)
{
    m_AttachedParts.push_back(partObject);
    // パーツ追加後にステータスを再集計・計算
    AggregateAndCalculateStats();
}

// パーツリストから全てのステータスを集計し、WeaponComponentのステータスを更新する
void WeaponComponent::AggregateAndCalculateStats()
{
    float totalAddedWeight = 0.0f;
    float totalMoment = 0.0f; // 重心計算用の重み付きモーメントの合計 (重心 * 重さ)
    int totalDamageBonus = 0;

    // パーツを集計
    for (auto& weakPart : m_AttachedParts) {
        if (auto partObject = weakPart.lock()) {
            // PartStatsComponentを取得し、静的ステータスを加算
            if (auto statsComp = partObject->GetComponent<WIS::PartStatsComponent>()) {
                float partWeight = statsComp->GetPartWeight();
                float partCenter = statsComp->GetCenterOfMassValue();

                totalAddedWeight += partWeight;
                totalMoment += partCenter * partWeight;
                totalDamageBonus += statsComp->GetDamageBonus();
            }
        }
    }

    // 最終的な重心の算出
    float finalCenterOfMass;
    if (totalAddedWeight > 0.0f) {
        // 重心 = 総モーメント / 総重量 (重み付き平均)
        // クランプして0.1から1.0の範囲に収める
        finalCenterOfMass = std::clamp(totalMoment / totalAddedWeight, 0.1f, 1.0f);
    }
    else {
        // パーツがない場合は基本重心を使用
        finalCenterOfMass = m_baseCenterOfMass;
    }

    // メンバー変数を更新
    m_currentAddedWeight = totalAddedWeight;
    m_currentCenterOfMass = finalCenterOfMass;
    m_currentDamageBonus = totalDamageBonus;

    // 最終ステータスを計算
    CalculateFinalStats();
}

// 攻撃力とクールダウン時間を最終計算する
void WeaponComponent::CalculateFinalStats()
{
    // 最終的な重さ (基本重量 + パーツ合計重量)
    float finalWeight = m_baseWeight + m_currentAddedWeight;
    // 最終的な重心 (パーツの重み付けで計算された値)
    float finalCenter = m_currentCenterOfMass;

    // 1. 最終攻撃力の計算 
    // (基本ダメージ + パーツボーナス) * (最終重さ * 最終重心)
    m_attackPower = (m_baseDamage + m_currentDamageBonus) * (finalWeight * finalCenter);

    // 2. クールダウン時間 (振りの遅さ) の計算 (重さ*重心に比例して遅くなる)
    float factor = finalWeight * finalCenter;
    float baseCoolDown = 0.2f;

    m_attackCoolDown = baseCoolDown * (1.0f + 0.5f * factor);

    // 最低クールダウン時間を設定
    if (m_attackCoolDown < 0.1f) {
        m_attackCoolDown = 0.1f;
    }
}

// 攻撃の実行を試みる関数
bool WeaponComponent::TryAttack()
{
    if (m_canAttack) {
        m_canAttack = false;
        m_coolDownTimer = 0.0f;

        // 攻撃が成功したら、振りの状態を開始し、ヒット判定を有効にする
        m_isSwinging = true;
        m_swingTimer = 0.0f;
        m_hasDealtDamageThisSwing = false; // 新しい振りの開始

        // 攻撃ヒットボックスの生成、アニメーション開始などのロジックをここに追加
        return true;
    }
    return false;
}