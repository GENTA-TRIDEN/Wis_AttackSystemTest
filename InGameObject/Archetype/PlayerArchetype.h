#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"
#include <string>

// 前方宣言（インクルード時間を短縮するため）
class ObjectManager;
class UserResources;
namespace WIS { class GameObject; }

namespace WIS {

    class PlayerArchetype : public WIS::ArchetypeBase
    {
    public:
        // コンストラクタはシンプルに、または削除して静的関数にしても良いです
        PlayerArchetype() = default;

        // オブジェクトを生成して、そのポインタを返す関数にします
        std::weak_ptr<WIS::GameObject> Create(
            ObjectManager* p_objectManager,
            UserResources* p_userResources,
            const std::string& BaseName, // 名前を受け取る（const参照推奨）
            int PlayerModelNum
        );

    private:
        // 定数は変更しないので const または static constexpr に
        const wchar_t* playerModelpath = L"Resources/Models/Player.sdkmesh";

        DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };
        DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f, 3.0f, 0.0f };
    };

}