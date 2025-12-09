#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// テンプレート関数内で ComponentBase のメンバ関数を呼ぶ必要があるため、ここはインクルードします。
// ※ ComponentBase.h 側で "GameObject.h" をインクルードしていないことが前提です！
#include "WISLib/GameObject/Component/ComponentBase.h"

// 前方宣言
class UserResources;

namespace WIS {

    // SceneTag がどこで定義されているかによりますが、
    // ここで前方宣言するか、適切なヘッダー（WISLibTags.hなど）をインクルードしてください。
    // エラーが出る場合は #include "WISLibTags.h" などを追加してください。


    class GameObject : public std::enable_shared_from_this<GameObject>
    {
    private:
        bool m_isActive = true;
        bool m_isStarted = false; // OnStartが呼ばれたかを管理するフラグ
        std::string m_name;

        // コンポーネントの所有権をshared_ptrで管理
        std::vector<std::shared_ptr<WIS::ComponentBase>> m_components;

        // 生ポインタで管理されていますが、デストラクタで解放するように修正しました
        std::vector<WIS::SceneTag>* m_pSceneTags = nullptr;

    public:
        GameObject();
        virtual ~GameObject();

        // --- コンポーネント管理 (テンプレート関数はヘッダーに残す必要があります) ---

        /// <summary>
        /// コンポーネントをこのゲームオブジェクトに追加
        /// </summary>
        template<typename T, typename... Args>
        std::shared_ptr<T> AddComponent(Args&&... args)
        {
            // T型のコンポーネントをshared_ptrとして生成
            auto newComponent = std::make_shared<T>(std::forward<Args>(args)...);

            // ComponentBaseのshared_ptrとしてコンテナに追加
            m_components.push_back(newComponent);

            // コンポーネントにオーナー(このGameObject)を教える
            newComponent->SetOwner(shared_from_this());

            return newComponent;
        }

        /// <summary>
        /// 指定された型のコンポーネントを取得
        /// </summary>
        template<typename T>
        std::shared_ptr<T> GetComponent()
        {
            for (const auto& component : m_components)
            {
                // dynamic_pointer_castを使って安全に型変換を試みる
                auto castedComponent = std::dynamic_pointer_cast<T>(component);
                if (castedComponent) {
                    return castedComponent; // 型が一致したらそのshared_ptrを返す
                }
            }
            return nullptr; // 見つからなければnullptrを返す
        }

        /// <summary>
        /// 指定された型のコンポーネントを削除
        /// </summary>
        template<typename T>
        void RemoveComponent()
        {
            auto it = std::remove_if(m_components.begin(), m_components.end(),
                [](const std::shared_ptr<WIS::ComponentBase>& component) {
                    // T型にキャストできるかどうかで判定
                    return dynamic_cast<T*>(component.get()) != nullptr;
                });
            m_components.erase(it, m_components.end());
        }

        // --- ライフサイクル関数 (実装は .cpp へ移動) ---
        void Start(UserResources* userResources);
        void PreUpdate(const float elapsedTime, UserResources* userResources);
        void Update(const float elapsedTime, UserResources* userResources);
        void PostUpdate(const float elapsedTime, UserResources* userResources);
        void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
        void OnRelease();

        // --- ゲッター/セッター (実装は .cpp へ移動) ---
        void SetName(const std::string& name);
        const std::string& GetName() const;

        void SetActive(bool isActive);
        bool CheckActive() const;

        // --- イベントディスパッチ ---
        void DispatchCollision(std::shared_ptr<GameObject> otherObject);

        void SetSceneTags(WIS::SceneTag sceneTags);
        std::vector<WIS::SceneTag>* GetSceneTags();
    };
}