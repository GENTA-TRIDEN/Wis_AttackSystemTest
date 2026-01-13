#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "GameComponent/ItemComponent.h"

namespace WIS {
	class GearItemComponent :public WIS::ItemComponent {
	private:
		

		WIS::ItemKinds m_itemType = WIS::ItemKinds::GEAR;

	public:

		GearItemComponent() :ItemComponent(WIS::ItemKinds::GEAR) {}



		//--Override関数--
		//クリエイト関数
		std::shared_ptr<ItemComponent> CreateItemComponent(WIS::ItemKinds itemKinds) override;



		//ItemComponentのオーバーライド
		void UniqueOnStart(UserResources* userResources) override;
		void UniqueUpdate(const float elapsedTime, UserResources* userResources) override;
		void UniqueOnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;
		void UniqueOnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;





	};





}

