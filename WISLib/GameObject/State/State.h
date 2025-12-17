#pragma once
#include <memory>
#include "UserResources.h"

// 前方宣言
namespace WIS {
    class GameObject;
}

namespace WIS {

    // 汎用ステートクラス
    // T: 操作対象のコンポーネント (EnemyComponentなど)
    template <typename T>
    class State
    {
    public:
        virtual ~State() = default;

        // ステート開始時
        virtual void OnEnter(T* owner) {}

        // 更新処理
        virtual void OnUpdate(T* owner, float elapsedTime, UserResources* userResources) {}

        // ステート終了時
        virtual void OnExit(T* owner) {}

        // 衝突判定
        virtual void OnCollision(T* owner, std::shared_ptr<WIS::GameObject> otherObject) {}
    };
}