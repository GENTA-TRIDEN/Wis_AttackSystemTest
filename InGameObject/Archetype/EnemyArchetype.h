#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"
#include <string>

// 前方宣言（インクルード時間を短縮するため）
class ObjectManager;
class UserResources;
namespace WIS { class GameObject; }

namespace WIS {

    class EnemyArchetype : public WIS::ArchetypeBase
    {
    public:
        // コンストラクタはシンプルに、または削除して静的関数にしても良いです
        EnemyArchetype() = default;

        // オブジェクトを生成して、そのポインタを返す関数にします
        std::weak_ptr<WIS::GameObject> Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int EnemyModelNumconst, std::string& floorName);


    private:

        const wchar_t* enemyModelpath = L"Resources/Models/Enemy.sdkmesh";

        DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };
        DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f, 3.0f, 0.0f };
    };

}
