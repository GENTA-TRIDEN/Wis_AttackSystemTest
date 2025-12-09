#pragma once
#include <memory> // std::shared_ptr, std::weak_ptr のため
#include "SimpleMath.h"


// 前方宣言
namespace WIS {
    class GameObject;
}

class UserResources;

namespace WIS{

    class ComponentBase
    {
    private:
        // このコンポーネントの所有者であるGameObjectへのポインタ
        // 循環参照を避けるため、weak_ptrで保持する
        std::weak_ptr<WIS::GameObject> m_owner;

    public:
        //  派生クラスのデストラクタが正しく呼ばれるように、必ず仮想デストラクタを定義する
        virtual ~ComponentBase() = default;

        // --- ライフサイクル仮想関数 ---
        virtual void OnStart(UserResources* userResources) {}
        virtual void OnPreUpdate(const float elapsedTime, UserResources* userResources) {}
        virtual void OnUpdate(const float elapsedTime, UserResources* userResources) {}
        virtual void OnPostUpdate(const float elapsedTime, UserResources* userResources) {}
        virtual void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) {}
        virtual void OnRelease() {}

        // 衝突イベント
        virtual void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) {}

        // --- オーナーのセッター/ゲッター ---
        void SetOwner(std::shared_ptr<WIS::GameObject> owner)
        {
            m_owner = owner;
        }

        std::shared_ptr<WIS::GameObject> GetOwner() const
        {
            return m_owner.lock(); // weak_ptrからshared_ptrを生成して返す
        }
    };

}