#include "pch.h"
#include "WeaponPartsComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WeaponComponent.h" // 親のWeaponComponentにアクセスするために必要

using namespace WIS;

// コンストラクタ: 親オブジェクトの参照を保持
// ユーザー提供コードに基づき実装
WIS::WeaponPartsComponent::WeaponPartsComponent(std::shared_ptr<WIS::GameObject> parentWeaponObject)
	: m_ParentWeaponObject(parentWeaponObject)
{
	// ここでのCalculateStats()呼び出しは、親武器の集計をトリガーするのが目的
	CalculateStats();
}

WeaponPartsComponent::~WeaponPartsComponent() {}

void WeaponPartsComponent::OnStart(UserResources* userResources)
{
	// OnStart時にも念のため親武器に再集計を促す
	CalculateStats();
}

void WeaponPartsComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	// ここにパーツ固有のUpdateロジック（例：エフェクトの点滅、耐久度チェック）を記述
}

void WeaponPartsComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
	if (auto parent = m_ParentWeaponObject.lock()) {
		if (auto weaponComp = parent->GetComponent<WIS::WeaponComponent>()) {
			// 親武器の再集計と再計算をトリガー
			weaponComp->OnCollision(otherObject);
		}
	}
}

// 武器の重さを増やす
void WeaponPartsComponent::AddWeight(float addedWeight)
{
	// この関数は主にWeaponComponent側のAggregateAndCalculateStats()内で処理されるため、
	// ここでは特にアクションは不要。
}

// 武器の重心を設定
void WeaponPartsComponent::SetCenterOfMass(float newCenterOfMass)
{
	// この関数は主にWeaponComponent側のAggregateAndCalculateStats()内で処理されるため、
	// ここでは特にアクションは不要。
}

// 攻撃力とクールダウン時間を計算する
// 親武器の集計・計算処理をトリガーする役割を持つ
void WeaponPartsComponent::CalculateStats()
{
	if (auto parent = m_ParentWeaponObject.lock()) {
		if (auto weaponComp = parent->GetComponent<WIS::WeaponComponent>()) {
			// 親武器の再集計と再計算をトリガー
			weaponComp->AggregateAndCalculateStats();
		}
	}
}

// 攻撃の実行を試みる関数
// 親武器のTryAttackに処理を委譲
bool WeaponPartsComponent::TryAttack()
{
	if (auto parent = m_ParentWeaponObject.lock()) {
		if (auto weaponComp = parent->GetComponent<WIS::WeaponComponent>()) {
			return weaponComp->TryAttack();
		}
	}
	return false;
}

// 現在の攻撃状態を取得
// 親武器に処理を委譲
std::string WeaponPartsComponent::GetAttackState()
{
	// 実際にはWeaponComponentに状態取得関数が必要
	// if (auto parent = m_ParentWeaponObject.lock()) { ... }
	return "UNKNOWN_STATE";
}