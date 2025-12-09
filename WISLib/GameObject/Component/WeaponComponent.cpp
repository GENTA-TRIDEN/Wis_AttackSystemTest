#include "WeaponComponent.h"
#include "CharacterComponent.h" // ダメージを与えるためにCharacterComponentの定義が必要
#include <algorithm> // std::clamp用
#include <cmath>     // std::pow, std::sqrt用

// 仮のインクルード (実際のプロジェクト構成に合わせて調整してください)

#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"


using namespace WIS;

WeaponComponent::WeaponComponent()
{
	CalculateStats();
}

WeaponComponent::~WeaponComponent() {}

void WeaponComponent::OnStart(UserResources* userResources)
{
	// 初期化処理
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

void WeaponComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
	// 武器が振られている最中であり、かつ、まだ今回の振りでダメージを与えていないかチェック
	if (m_isSwinging && !m_hasDealtDamageThisSwing)
	{
		// 衝突相手からCharacterComponentを取得
		auto TargetCharacter = otherObject->GetComponent<WIS::CharacterComponent>();

		if (TargetCharacter)
		{
			// 敵オブジェクトであることを確認
			if (TargetCharacter->GetObjectType() == WIS::ObjectType::ENEMY)
			{
				// 攻撃力を渡して体力を減らす（※CharacterComponentのDecreaseHPを使用するのが一般的）
				// ユーザーの元のロジックに合わせて IncreaseHP() を使用していますが、一般的には DecreaseHP() です。
				// TargetCharacter->DecreaseHP(m_attackPower); // 本来推奨される処理

				TargetCharacter->DecreaseHP(m_attackPower); // ユーザーの元コードが IncreaseHP だったので、引数に合わせて DecreaseHP に修正

				// ダメージを与えたのでフラグを立て、この振りではこれ以上ダメージを与えないようにする
				m_hasDealtDamageThisSwing = true;
			}
		}
	}
}

// 武器の重さを増やす
void WeaponComponent::AddWeight(float addedWeight)
{
	m_currentWeight += addedWeight;
	if (m_currentWeight < m_baseWeight) {
		m_currentWeight = m_baseWeight;
	}
	CalculateStats();
}

// 武器の重心を設定
void WeaponComponent::SetCenterOfMass(float newCenterOfMass)
{
	m_currentCenterOfMass = std::clamp(newCenterOfMass, 0.1f, 1.0f);
	CalculateStats();
}

// 攻撃力とクールダウン時間を計算する
void WeaponComponent::CalculateStats()
{
	// 1. 最終攻撃力の計算 
	m_attackPower = m_baseDamage * (m_currentWeight * m_currentCenterOfMass);

	// 2. クールダウン時間 (振りの遅さ) の計算 (憶測に基づいたロジック)
	float factor = m_currentWeight * m_currentCenterOfMass;
	float baseCoolDown = 0.2f;

	m_attackCoolDown = baseCoolDown * (1.0f + 0.5f * factor);

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