#include "pch.h"
#include "PlayerArchetype.h"
#include "UserResources.h"
#include "WISLib/GameObject/ObjectManager.h"


#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"

#include "GameComponent/PlayerComponent.h"


std::weak_ptr<WIS::GameObject> WIS::PlayerArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int PlayerModelNum)
{

    // 名前からオブジェクトを生成
    // GenerateObjectが名前を変更する場合を考慮して戻り値を受け取る形はOKです
    auto generatedObj = p_objectManager->GenerateObject(BaseName, WIS::SceneTag::GAMEPLAY_SCENE);
    std::string finalName = generatedObj->GetName();

    // デバイス関連の取得
    auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
    auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
    auto fxFactory = p_userResources->GetEffectFactory();

    // playerの取得
    auto player = p_objectManager->GetObject(finalName);

    // --- コンポーネントの追加 ---
    auto playerColl = player->AddComponent<WIS::CollisionComponent>();
    auto PlayerComp = player->AddComponent<WIS::PlayerComponent>();
    auto PlayerCharacter = player->AddComponent<WIS::CharacterComponent>();
    auto PlayerRigidBody = player->AddComponent<WIS::RigidBodyComponent>();
    auto PlayerModel = player->AddComponent<WIS::ModelComponent>();
    auto PlayerTransform = player->GetComponent<WIS::TransformComponent>(); // Transformは最初からある前提ならこれでOK


    // モデルロード
    int playerHandle = PlayerModel->AddModel(device, context, playerModelpath, fxFactory);
    PlayerModel->SetRotate(playerHandle, { 0, DirectX::XMConvertToRadians(-90), 0 });
    PlayerModel->SetPosition(playerHandle, ModelPos);

    // 座標とコリジョン設定
    PlayerTransform->SetPosition(SpawnPos);
    playerColl->AddBoundingSphere(1.0f, { 0.0f, 0.0f, 0.0f });

    // キャラクタータイプ設定
    PlayerCharacter->SetObjectType(WIS::ObjectType::PLAYER);


    //オブジェクトを返す
    return player;


}
