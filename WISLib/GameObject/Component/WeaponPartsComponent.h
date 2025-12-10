#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <memory>
#include <vector>
#include <string>

namespace WIS {
	// 前方宣言
	class GameObject;
	class WeaponComponent;

	class WeaponPartsComponent : public WIS::ComponentBase
	{
	private:
		//--- 武器パーツ管理 ---
		// このパーツ自体が、さらにサブパーツを持つ場合のコンテナ
		std::vector<std::shared_ptr<WIS::GameObject>> m_MyParts;

		// 親武器オブジェクトの参照 (主にWeaponComponentへのアクセスに使用)
		std::weak_ptr<WIS::GameObject> m_ParentWeaponObject;

	public:
		// コンストラクタで親武器オブジェクトを設定
		WeaponPartsComponent(std::shared_ptr<WIS::GameObject> parentWeaponObject);
		~WeaponPartsComponent();

		//--- ComponentBaseのオーバーライド関数 ---
		void OnStart(UserResources* userResources) override;
		void OnUpdate(const float elapsedTime, UserResources* userResources) override;
		// 衝突処理は親武器のWeaponComponentに任せることが多いため、ここではロジックを持たない
		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;

		//--- ステータス調整関数 (WeaponComponentの機能と重複するため、ここでは空実装または委譲) ---
		void AddWeight(float addedWeight);
		void SetCenterOfMass(float newCenterOfMass);
		// このコンポーネントは集計ロジックを持たないため、親の集計をトリガーする役割を担うべき
		void CalculateStats();


		// 攻撃は親武器のTryAttackに委譲
		bool TryAttack();
		// 攻撃状態の取得も親武器に委譲
		std::string GetAttackState();

		// サブパーツを追加する
		void AddPart(std::shared_ptr<WIS::GameObject> part) {
			m_MyParts.push_back(part);
		}
	};
}