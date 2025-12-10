#pragma once
#include "GameObject.h"
#include <string>
#include <list>
#include <unordered_map>
#include <memory>

// UserResourcesの前方宣言
class UserResources;

namespace WIS {
    class GameObject;
}

namespace WIS {
    using Objects = std::vector< std::shared_ptr<WIS::GameObject>>;
    // 全てのGameObjectを管理するクラス
    class ObjectManager
    {
    public:
        ObjectManager();
        ~ObjectManager();

        std::shared_ptr<WIS::GameObject> GenerateObject(std::string a_name, WIS::SceneTag SceneTags);
        std::shared_ptr<WIS::GameObject> GetObject(std::string a_name);

        //オブジェクトのＳｃｅｎｅタグを追加する関数
        void AddObjectSceneTag(std::string a_name, WIS::SceneTag sceneTag);

        // 更新関数
        void Update(const float elapsedTime, UserResources* userResources);

        // 描画関数
        void Render(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj);

        // 終了処理
        void Finalize();

        void SetCurrentSceneTag(WIS::SceneTag sceneTag) {
            m_currentSceneTag = sceneTag;
        };

        WIS::SceneTag GetCurrentSceneTag() {
            return m_currentSceneTag;
        };

        void AddObject(std::shared_ptr<WIS::GameObject> object) {
            if (!object) return;
            //まだないかを確認してから追加
            if (m_umNameToObject.find(object->GetName()) == m_umNameToObject.end()) {
                m_lObjects.push_back(object);
                m_umNameToObject[object->GetName()] = object;
            }
        }

    private:
        // 引数の名前を元に被らない名前を作成する
        std::string CreateObjName(std::string a_baseName);

        //  この行を追加 
        void StartObjects(UserResources* userResources);
        // 各更新フェーズ
        void PreUpdateObjects(const float elapsedTime, UserResources* userResources);
        void UpdateObjects(const float elapsedTime, UserResources* userResources);
        void PostUpdateObjects(const float elapsedTime, UserResources* userResources);

        // 無効なオブジェクトを全て削除する
        void RemoveUnActiveObjects();

        void RenderObjects(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj);

        void FinalizeObjects();

    private:
        // 全てのオブジェクトのインスタンスを格納するコンテナ
        std::list<std::shared_ptr<WIS::GameObject>> m_lObjects;

        // 名前とshared_ptrを直接紐づけるコンテナ (イテレータより堅牢)
        std::unordered_map<std::string, std::shared_ptr<WIS::GameObject>> m_umNameToObject;

        WIS::SceneTag m_currentSceneTag;


        //ObjectsをSceneタグごとに管理するためのマップ
        std::unordered_map<WIS::SceneTag, Objects> m_sceneTagToObjects;


        //↓しないほうがいいため消す
    ////シングルトン
    //private:
    //    //シングルトンにするためのunique_ptr
    //    static std::unique_ptr<ObjectManager> s_objectManager;

    };

}