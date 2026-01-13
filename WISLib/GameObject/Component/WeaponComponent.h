#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include "WISLib/UserInterface/HitEffect.h"
#include <memory>
#include <vector>
#include <algorithm> // std::clamp用



namespace WIS {
	// 前方宣言: 他のコンポーネントやゲームオブジェクト
	class GameObject;
	class PartStatsComponent;

	class WeaponComponent : public WIS::ComponentBase
	{
	private:
		//武器の基礎データ 
		int m_baseDamage = 10;		    // 武器の基本攻撃力
		float m_baseWeight = 1.0f;	    // 武器の基本重量
		float m_baseCenterOfMass = 0.5f; // 武器の基本重心

		//パーツの影響による計算値 
		float m_currentAddedWeight = 0.0f;		     // パーツから合計された追加重量
		float m_currentCenterOfMass = 0.5f;          // パーツの影響で計算された最終重心 (0.1～1.0)
		int m_currentDamageBonus = 0;                // パーツから合計された追加ダメージ

		//攻撃時の最終ステータス
		float m_attackPower = 0.0f;		    // 最終攻撃力
		float m_attackCoolDown = 0.5f;	    // 最終クールダウン時間

		//攻撃状態管理
		float m_coolDownTimer = 0.0f;
		bool m_canAttack = true;
		bool m_isSwinging = false;             
		bool m_hasDealtDamageThisSwing = false;  // 今回の振りで既にダメージを与えたか
		const float m_swingDuration = 0.8f;      // 振りの持続時間
		float m_swingTimer = 0.0f;


		//Chargeアタック管理
		bool m_isCharging = false;
		float m_chargeTimer = 0.0f;
		const float m_maxChargeTime = 2.0f;
		const float m_minChargeTime = 0.5f;



		int m_chargeLevel = 0; //0:通常 1:中 2:大
		const float m_chargeCoolDownMultiplier[3] = { 1.0f, 1.5f, 2.0f }; //チャージレベルごとのクールダウン倍率
		const float m_chargeDamageMultiplier[3] = { 1.0f, 1.5f, 2.0f }; //チャージレベルごとのダメージ倍率


		

		//左右に振る
		//
		bool m_IsLeftToRight = true;



		AttackType m_currentAttackType = AttackType::NORMAL;

	



		// 武器を構成するパーツオブジェクトのリスト (弱参照で循環参照を回避)
		std::vector<std::weak_ptr<WIS::GameObject>> m_AttachedParts;
		WIS::HitEffect* p_Effect;

	public:
		WeaponComponent();
		~WeaponComponent();

		// ComponentBaseのオーバーライド関数 
		void OnStart(UserResources* userResources) override;
		void OnUpdate(const float elapsedTime, UserResources* userResources) override;

		void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)override;
		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;


		void AddWeight(float weight) {
			m_currentAddedWeight += weight;
		}
		// パーツ管理関数 
		// パーツオブジェクトをリストに追加
		void AttachPart(std::shared_ptr<WIS::GameObject> partObject);

		// パーツリストから全てのステータスを集計し、WeaponComponentのステータスを更新する
		void AggregateAndCalculateStats();

		// 攻撃制御関数 
		// 攻撃を試み、可能であれば実行してクールダウンを開始する
		bool TryAttack();
		
		//3段階の溜めがある攻撃
		bool ChargeAttack(float elapsedTime);
		bool TryAttackHeavy();

		// 最終ステータスを計算する
		void CalculateFinalStats();

		void SetEffect(WIS::HitEffect* Effect) { p_Effect = Effect; };

		float GetWeight() const {
			return m_baseWeight + m_currentAddedWeight;
		}
		float GetBaseWeight() const {
			return m_baseWeight;
		}

		WIS::AttackType GetCurrentAttackType() const {
			return m_currentAttackType;
		}

	};
}