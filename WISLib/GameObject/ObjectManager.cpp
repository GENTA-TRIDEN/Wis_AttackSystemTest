#include "pch.h"
#include "ObjectManager.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/WISLibTags.h"
#include "UserResources.h"

WIS::ObjectManager::ObjectManager() {
    m_lObjects.clear();
}
WIS::ObjectManager::~ObjectManager() {}

//// シングルトンインスタンスの初期化
//std::unique_ptr<WIS::ObjectManager> WIS::ObjectManager::s_objectManager = nullptr;

// 引数の名前のオブジェクトを作成して返す関数
std::shared_ptr<WIS::GameObject> WIS::ObjectManager::GenerateObject(std::string a_name, WIS::SceneTag SceneTags)
{
    auto spNewObject = std::make_shared<WIS::GameObject>();

    std::string objName = CreateObjName(a_name);

    spNewObject->SetName(objName);
    spNewObject->SetActive(true);
    spNewObject->SetSceneTags(SceneTags);
    spNewObject->AddComponent<WIS::TransformComponent>();

    m_lObjects.push_back(spNewObject);
    m_umNameToObject[objName] = spNewObject;

    if (spNewObject) {
        m_sceneTagToObjects[SceneTags].push_back(spNewObject);
    }

    return spNewObject;
}

// 名前からオブジェクトを取得する
std::shared_ptr<WIS::GameObject> WIS::ObjectManager::GetObject(std::string a_name)
{
    auto itr = m_umNameToObject.find(a_name);
    if (itr != m_umNameToObject.end()) {
        return itr->second;
    }
    return nullptr;
}

void WIS::ObjectManager::AddObjectSceneTag(std::string a_name, WIS::SceneTag sceneTag)
{
    auto object = GetObject(a_name);

    if (object) {
        m_sceneTagToObjects[sceneTag].push_back(object);
    }
}

// 更新関数
void WIS::ObjectManager::Update(const float elapsedTime, UserResources* userResources)
{

    StartObjects(userResources);


    PreUpdateObjects(elapsedTime, userResources);
    UpdateObjects(elapsedTime, userResources);
    PostUpdateObjects(elapsedTime, userResources);
    RemoveUnActiveObjects();
}

// 描画関数
void WIS::ObjectManager::Render(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    RenderObjects(userResources, view, proj);
}

// 終了処理
void WIS::ObjectManager::Finalize()
{
    FinalizeObjects();


    //// シングルトンインスタンスの破棄
    //s_objectManager.reset();

}

// 被らない名前を作成する
std::string WIS::ObjectManager::CreateObjName(std::string a_baseName)
{
    if (m_umNameToObject.find(a_baseName) == m_umNameToObject.end()) {
        return a_baseName;
    }

    for (size_t i = 1; ; ++i) {
        std::string newName = a_baseName + "+" + std::to_string(i);
        if (m_umNameToObject.find(newName) == m_umNameToObject.end()) {
            return newName;
        }
    }
}

// --- プライベートヘルパー関数 ---

//  この関数を丸ごと追加 
void WIS::ObjectManager::StartObjects(UserResources* userResources)
{
    for (auto& object : m_lObjects) {
        if (object) {
            object->Start(userResources);
        }
    }
}

void WIS::ObjectManager::PreUpdateObjects(const float elapsedTime, UserResources* userResources)
{
    Objects CurrentObjects = m_sceneTagToObjects[m_currentSceneTag];

    for (auto& object : CurrentObjects) {
        object->PreUpdate(elapsedTime, userResources);
    }
}

void WIS::ObjectManager::UpdateObjects(const float elapsedTime, UserResources* userResources)
{
    Objects CurrentObjects = m_sceneTagToObjects[m_currentSceneTag];

    for (auto& object : CurrentObjects) {
        if (object) object->Update(elapsedTime, userResources);
    }

}

void WIS::ObjectManager::PostUpdateObjects(const float elapsedTime, UserResources* userResources)
{
    Objects CurrentObjects = m_sceneTagToObjects[m_currentSceneTag];

    for (auto& object : CurrentObjects) {
        if (object) object->PostUpdate(elapsedTime, userResources);
    }


}

void WIS::ObjectManager::RenderObjects(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    //for (auto& object : m_lObjects) {
    //    if (object) object->OnRender(userResources, view, proj);
    //}
    Objects CurrentObjects = m_sceneTagToObjects[m_currentSceneTag];

    for (auto& object : CurrentObjects) {
        if (object) object->OnRender(userResources, view, proj);
    }



}

// 無効なオブジェクトを全て削除する
void WIS::ObjectManager::RemoveUnActiveObjects()
{
    std::vector<std::string> namesToRemove;
    for (auto& object : m_lObjects) {
        if (object && !object->CheckActive()) {
            object->OnRelease();
            namesToRemove.push_back(object->GetName());
        }
    }

    for (const auto& name : namesToRemove) {
        m_umNameToObject.erase(name);
    }

    m_lObjects.remove_if([](const std::shared_ptr<GameObject>& obj) {
        return !obj || !obj->CheckActive();
        });
}

// 全てのオブジェクトの終了処理
void WIS::ObjectManager::FinalizeObjects()
{

    //総当たり
    for (auto itr = m_sceneTagToObjects.begin(); itr != m_sceneTagToObjects.end(); ++itr)
    {
        WIS::Objects obj = itr->second;
        for (std::shared_ptr<WIS::GameObject> sharedObj : obj) {
            sharedObj->OnRelease();
            sharedObj.reset();
        }

    }

    m_lObjects.clear();

    for (auto& object : m_lObjects) {
        if (object) object->OnRelease();
    }


    m_lObjects.clear();
    m_umNameToObject.clear();



}