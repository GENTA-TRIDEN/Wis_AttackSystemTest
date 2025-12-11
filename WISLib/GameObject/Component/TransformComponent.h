#pragma once
#include "ComponentBase.h"
#include <memory>
#include <vector>

namespace WIS {

    class TransformComponent : public ComponentBase, public std::enable_shared_from_this<TransformComponent>
    {
    private:
        // ローカル変形
        DirectX::SimpleMath::Vector3 m_position;
        DirectX::SimpleMath::Quaternion m_rotation;
        DirectX::SimpleMath::Vector3 m_scale;

        // 親子関係
        std::weak_ptr<TransformComponent> m_parent;
        std::vector<std::shared_ptr<TransformComponent>> m_children;

        // パフォーマンス最適化のためのキャッシュ
        mutable DirectX::SimpleMath::Matrix m_worldMatrix;
        mutable bool m_isDirty; 

        void SetDirty(); // 自分とすべての子をダーティに設定する

    public:
        TransformComponent();
        ~TransformComponent() = default;

        void OnStart(UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;

        // --- ローカル変形へのアクセス ---
        const DirectX::SimpleMath::Vector3& GetPosition() const { return m_position; }
        const DirectX::SimpleMath::Quaternion& GetRotation() const { return m_rotation; }
        const DirectX::SimpleMath::Vector3& GetScale() const { return m_scale; }

        void SetPosition(const DirectX::SimpleMath::Vector3& position);
        void SetRotation(const DirectX::SimpleMath::Quaternion& rotation);
        void SetScale(const DirectX::SimpleMath::Vector3& scale);
        void SetWorld(const DirectX::SimpleMath::Matrix mat);

        void Translate(const DirectX::SimpleMath::Vector3& offset);

        // --- 行列の取得 ---
        DirectX::SimpleMath::Matrix GetLocalMatrix() const;
        const DirectX::SimpleMath::Matrix& GetWorldMatrix() const;

        // --- 親子関係の操作 ---
        void SetParent(std::shared_ptr<TransformComponent> newParent);
        std::shared_ptr<TransformComponent> GetParent() const { return m_parent.lock(); }


        DirectX::SimpleMath::Vector3 GetForward() const;
        DirectX::SimpleMath::Vector3 GetRight() const;
        DirectX::SimpleMath::Vector3 GetUp() const;

    private:
        // SetParentから内部的に呼び出される
        void AddChild(std::shared_ptr<TransformComponent> child);
        void RemoveChild(std::shared_ptr<TransformComponent> child);
    };

}
