#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"
#include <string>

// 前方宣言（インクルード時間を短縮するため）
class ObjectManager;
class UserResources;
namespace WIS { class GameObject; }

namespace WIS {

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
			DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f,3.0f,0.0f }
        );


    private:

        const wchar_t* PartsModelPath = L"Resources/Models/Enemy.sdkmesh";


        const wchar_t* GetItemModelPath(WIS::ItemKinds itemKinds);
        DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };
		DirectX::SimpleMath::Vector3 Rotate = { 0.0f,0.0f,0.0f };
    };

}
