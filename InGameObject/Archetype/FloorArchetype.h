#pragma once
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"
#include <string>

namespace WIS {

class FloorArchetype :public WIS::ArchetypeBase
{
public:
	FloorArchetype() = default;

	// オブジェクトを生成して、そのポインタを返す関数にします

	/// <summary>
	/// 床オブジェクトを生成する
	/// </summary>
	/// <param name="p_objectManager">オブジェクトマネージャーポインタ</param>
	/// <param name="p_userResources">ユーザーリソースポインタ</param>
	/// <param name="BaseName">FloorName</param>
	/// <param name="FloorModelNumconst">FloorModelのHandle</param>
	/// <param name="playerName">PlayerName</param>
	/// <returns></returns>
	std::weak_ptr<WIS::GameObject> Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int FloorModelNumconst, const std::string& playerName);

private:
	const wchar_t* floorModelPath = L"Resources/Models/Stage3.sdkmesh";
	//const wchar_t* floorModelPath = L"Resources/Models/Floor.sdkmesh";

	DirectX::SimpleMath::Vector3 ModelPos = { 0.0f, 1.0f, 0.0f };  
	DirectX::SimpleMath::Vector3 SpawnPos = { 0.0f, 3.0f, 0.0f };
	DirectX::SimpleMath::Vector3 ModelScale = { 15.0f,15.0f,15.0f };

};



}