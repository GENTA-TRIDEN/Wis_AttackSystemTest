#pragma once
#include "WISLib/WISLibTags.h"
#include "WISLib/GameObject/Component/ComponentBase.h"

namespace WIS{

	class ItemComponent :public WIS::ComponentBase
{
private:

	WIS::ItemKinds m_itemType;

	float m_ItemRadius =1.0f;

public:
	ItemComponent(WIS::ItemKinds itemType) :m_itemType(itemType) {}
	WIS::ItemKinds GetItemType() const { return m_itemType; }

	// ComponentBaseのオーバーライド（使用しないが定義は必要）
	void OnStart(UserResources* userResources) override;
	void OnUpdate(const float elapsedTime, UserResources* userResources) override;
	void OnCollision(std::shared_ptr<WIS::GameObject> otherObject)override;


	void SetItemKinds(WIS::ItemKinds itemType) { m_itemType = itemType; }
	WIS::ItemKinds GetItemKinds() const { return m_itemType; }
};

}