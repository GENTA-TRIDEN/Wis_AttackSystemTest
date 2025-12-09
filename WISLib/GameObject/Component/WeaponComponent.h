#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <memory>

namespace WIS {
	class WeaponComponent : public WIS::ComponentBase
	{
	private:
		//--- 武器の基礎データ ---
		int m_baseDamage = 10;		    // 武器の基本攻撃力
		float m_baseWeight = 1.0f;	    // 武器の基本重量 (最小重量)
		float m_baseCenterOfMass = 0.5f; // 武器の基本重心 (手元:0.1 ～ 先端:1.0)

		//--- 外部要素で調整された現在のステータス ---
		float m_currentWeight = m_baseWeight;		    // パーツ等で増減した現在の総重量
		float m_currentCenterOfMass = m_baseCenterOfMass; // パーツ等で調整された現在の重心

		//--- 攻撃時の計算値 ---
		float m_attackPower = 0.0f;		    // 最終的な攻撃力 (重さ*重心を考慮)
		float m_attackCoolDown = 0.5f;	    // 攻撃後のクールダウン時間 (振りの速さに相当)

		//--- 攻撃状態管理 ---
		float m_coolDownTimer = 0.0f;	        // クールダウン経過時間用のタイマー
		bool m_canAttack = true;		        // 次の攻撃が可能かどうかのフラグ

		// ⭐ 新規導入: 武器を振っている最中か (ヒット判定有効期間)
		bool m_isSwinging = false;
		// ⭐ 新規導入: 現在の振りで既にダメージを与えたか
		bool m_hasDealtDamageThisSwing = false;

		// 仮のヒット判定期間 (アニメーション時間に相当)
		const float m_swingDuration = 0.3f;
		float m_swingTimer = 0.0f;


	public:
		WeaponComponent();
		~WeaponComponent();

		//--- ComponentBaseのオーバーライド関数 ---
		void OnStart(UserResources* userResources) override;
		void OnUpdate(const float elapsedTime, UserResources* userResources) override;
		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;

		//--- ステータス調整関数 ---
		void AddWeight(float addedWeight);
		void SetCenterOfMass(float newCenterOfMass);
		void CalculateStats();

		//--- 取得関数 ---
		float GetAttackPower() { return m_attackPower; }
		float GetAttackCoolDown() { return m_attackCoolDown; }

		// 攻撃を試み、可能であれば実行してクールダウンを開始する
		bool TryAttack();
	};
}