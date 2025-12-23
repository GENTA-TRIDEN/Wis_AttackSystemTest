#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"

namespace WIS
{

class WeapoArchetype :public WIS::ArchetypeBase
{


public:
	WeapoArchetype() = default;

    // オブジェクトを生成して、そのポインタを返す関数にします
    std::weak_ptr<WIS::GameObject> Create(
        ObjectManager* p_objectManager,
        UserResources* p_userResources,
        const std::string& BaseName, // 名前を受け取る（const参照推奨）
        int WeaponModelNum,
        const std::string& PlayerName
    );

private:
    const wchar_t* weaponModelpath = L"Resources/Models/Weapon/Stick.sdkmesh";

    DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };
    DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f, 3.0f, 0.0f };
    DirectX::SimpleMath::Vector3 Scale = { 0.7f, 0.7f, 0.7f };





	
};

}
