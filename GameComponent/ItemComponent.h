#pragma once
#include "WISLib/WISLibTags.h"
#include "WISLib/GameObject/Component/ComponentBase.h"

namespace WIS{

	class ItemComponent :public WIS::ComponentBase
{
private:

	WIS::ItemKinds m_itemType;

	float m_ItemRadius =1.0f;

	float m_timer = 0;
	const float m_camGettingInterval=1.2f;
public:
	ItemComponent(WIS::ItemKinds itemType) :m_itemType(itemType) {}
	WIS::ItemKinds GetItemType() const { return m_itemType; }

	// ComponentBaseのオーバーライド（使用しないが定義は必要）
	void OnStart(UserResources* userResources) override;
	void OnUpdate(const float elapsedTime, UserResources* userResources) override;
	void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)override;
	void OnCollision(std::shared_ptr<WIS::GameObject> otherObject)override;

	//GetItemModelPath関数
	const wchar_t* GetItemModelPath()
	{
		switch (m_itemType)
		{
		case WIS::ItemKinds::BLADE:
			return L"Resources/Models/Weapon/Blade.sdkmesh";
		case WIS::ItemKinds::BOMB:
			return L"Resources/Models/Weapon/Bomb.sdkmesh";
		case WIS::ItemKinds::ARROW:
			return L"Resources/Models/Weapon/Arrow.sdkmesh";
		case WIS::ItemKinds::GEAR:
			return L"Resources/Models/Weapon/Gear.sdkmesh";
		default:
			return L"Resources/Models/Weapon/Gear.sdkmesh";
		}
		return L"";
	}



	void SetItemKinds(WIS::ItemKinds itemType) { m_itemType = itemType; }
	WIS::ItemKinds GetItemKinds() const { return m_itemType; }
	float GetItemRadius() const { return m_ItemRadius; }
	void SetItemRadius(float radius) { m_ItemRadius = radius; }


	//クリエイト関数
	virtual std::shared_ptr<ItemComponent> CreateItemComponent(WIS::ItemKinds itemKinds) = 0;

	
	//派生クラスのための仮想関数
	virtual void UniqueOnStart(UserResources* userResources) = 0;
	virtual void UniqueUpdate(const float elapsedTime, UserResources* userResources) = 0;
	virtual void UniqueOnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) = 0;
	virtual void UniqueOnCollision(std::shared_ptr<WIS::GameObject> otherObject) = 0;



};

}