#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"

namespace WIS {
	class CharacterComponent :public WIS::ComponentBase
	{
	private:
		
		int HP;			//体力
		int MaxHP;		//最大体力


		//攻撃を受けた後の無敵時間
		float invincibleTime = 2.0f;
		//経過時間を測る変数
		float invincibleTimer = 0.0f;
		bool isInvincible = false; //無敵状態かどうかのフラグ
		bool isHit = false; //攻撃を受けたかどうかのフラグ

		bool isCanGiveDamage = true; //ダメージを与えられるかどうかのフラグ


		ObjectType m_objectType = ObjectType::PLAYER; //オブジェクトの種類

	public:
		CharacterComponent();
		~CharacterComponent();
		void OnStart(UserResources* userResources) override;

		void OnUpdate(const float elapsedTime, UserResources* userResources) override;

		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;
		//体力を減らす関数
		void DecreaseHP(int damage);
		//体力を増やす関数
		void IncreaseHP(int heal);
		//体力を取得する関数
		int GetHP() { return HP; }
		//最大体力を取得する関数
		int GetMaxHP() { return MaxHP; }

		//攻撃を受ける関数
		void TakeAttack(int damage);

		



		void SetObjectType(ObjectType type) {
			m_objectType = type;
		}
		ObjectType GetObjectType() {
			return m_objectType;
		}
	};
}

