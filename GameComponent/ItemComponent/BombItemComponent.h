#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "GameComponent/ItemComponent.h"

//Effect用

#include "WISLib/UserInterface/BombEffect.h"

namespace WIS {
	class BombItemComponent :public WIS::ItemComponent {
	private:
		

		WIS::ItemKinds m_itemType = WIS::ItemKinds::BOMB;

		//BombEffect
		std::unique_ptr<WIS::BombEffect> m_Bombeffect;




	public:

		BombItemComponent() :ItemComponent(WIS::ItemKinds::BOMB) {}



		//--Override関数--
		//クリエイト関数
		std::shared_ptr<ItemComponent> CreateItemComponent(WIS::ItemKinds itemKinds) override;



		//ItemComponentのオーバーライド
		void UniqueOnStart(UserResources* userResources) override;
		void UniqueUpdate(const float elapsedTime, UserResources* userResources) override;
		void UniqueOnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
		void UniqueOnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;



		void SpawnExplosion(UserResources* resources);


	};





}

