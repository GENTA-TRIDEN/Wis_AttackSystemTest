#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"

#include <string>

// 前方宣言（インクルード時間を短縮するため）
class ObjectManager;
class UserResources;
namespace WIS { class GameObject; }

namespace WIS {
	class ItemComponent;

    class PartsArchetype : public WIS::ArchetypeBase
    {
    public:
        // コンストラクタはシンプルに、または削除して静的関数にしても良いです
        PartsArchetype() = default;

        // オブジェクトを生成して、そのポインタを返す関数にします
        std::weak_ptr<WIS::GameObject> Create(
            ObjectManager* p_objectManager,
            UserResources* p_userResources,
            const std::string& BaseName,
            int PartsModelNumconst,
            const std::vector <std::string>& m_itemNames,
            WIS::ItemKinds itemKinds,
			DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f,3.0f,0.0f },
            DirectX::SimpleMath::Vector3 SpawnImpact = { 0.0f,0.0f,0.0f }
        );


    private:

        std::shared_ptr<WIS::ItemComponent> AddItemComponent(std::shared_ptr<WIS::GameObject> obj,WIS::ItemKinds itemKinds);
        
        DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };
		DirectX::SimpleMath::Vector3 Rotate = { 0.0f,0.0f,0.0f };
    };

}
