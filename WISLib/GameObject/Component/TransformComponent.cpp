#include "pch.h"
#include "TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include <algorithm>

using namespace WIS;
using namespace DirectX::SimpleMath;

TransformComponent::TransformComponent() :
    m_position(Vector3::Zero),
    m_rotation(Quaternion::Identity),
    m_scale(Vector3::One),
    m_worldMatrix(Matrix::Identity),
    m_isDirty(true) // 最初は必ず計算する必要がある
{
}

void TransformComponent::OnStart(UserResources* /*userResources*/) {}

void TransformComponent::OnUpdate(const float /*elapsedTime*/, UserResources* /*userResources*/) {}

void TransformComponent::SetPosition(const Vector3& position)
{
    m_position = position;
    SetDirty();
}

void TransformComponent::SetRotation(const Quaternion& rotation)
{
   auto owner= GetOwner();
    m_rotation = rotation;
    SetDirty();
}

void TransformComponent::SetScale(const Vector3& scale)
{
    m_scale = scale;
    SetDirty();
}

void WIS::TransformComponent::SetWorld(const DirectX::SimpleMath::Matrix mat)
{
    m_worldMatrix = mat;
    SetDirty();

}

void TransformComponent::Translate(const Vector3& offset)
{
    m_position += offset;
    SetDirty();
}

// ローカル行列（親から見た相対的な変形行列）を計算
Matrix TransformComponent::GetLocalMatrix() const
{
    return Matrix::CreateScale(m_scale) *
        Matrix::CreateFromQuaternion(m_rotation) *
        Matrix::CreateTranslation(m_position);
}

// ワールド行列（3D空間での絶対的な変形行列）を取得
const Matrix& TransformComponent::GetWorldMatrix() const
{
    // m_isDirtyフラグがtrueの場合のみ、ワールド行列を再計算
    if (m_isDirty)
    {
        const Matrix localMatrix = GetLocalMatrix();
        if (auto parent = m_parent.lock())
        {
            // 親がいる場合は、親のワールド行列に自身のローカル行列を掛け合わせます。
            m_worldMatrix = localMatrix * parent->GetWorldMatrix();
        }
        else
        {
            // 親がいない場合は、ローカル行列がそのままワールド行列になります。
            m_worldMatrix = localMatrix;
        }
        // 計算が終わったので、フラグをfalseに戻
        m_isDirty = false;
    }
    return m_worldMatrix;
}

// 自分自身の変形が変わったときに呼び出され、ワールド行列の再計算が必要なことをマーク
void TransformComponent::SetDirty()
{
    // 既にダーティなら何もしない
    if (m_isDirty) return;

    m_isDirty = true;
    // すべての子も再計算が必要になるため、再帰的にダーティフラグを立てます。
    for (auto& child : m_children)
    {
        if (child) child->SetDirty();
    }
}

// 親を設定nullptrを渡すと親子関係を解除
void TransformComponent::SetParent(std::shared_ptr<TransformComponent> newParent)
{
    // 既存の親から自分を削除する
    if (auto oldParent = m_parent.lock())
    {
        oldParent->RemoveChild(shared_from_this());
    }

    m_parent = newParent;

    // 新しい親に自分を追加する
    if (newParent)
    {
        newParent->AddChild(shared_from_this());
    }

    // 親が変わったので、ワールド行列の再計算が必要
    SetDirty();
}

// 子を追加
void TransformComponent::AddChild(std::shared_ptr<TransformComponent> child)
{
    m_children.push_back(child);
}

// 子を削除
void TransformComponent::RemoveChild(std::shared_ptr<TransformComponent> child)
{
    // std::removeは指定された値と等しい要素をコンテナの末尾に移動させ、
    // その末尾の開始イテレータを返す。eraseでそれらを実際に削除する。(erase-removeイディオム)
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}



Vector3 TransformComponent::GetForward() const
{
    // 回転(Quaternion)を使って、世界の前方(0,0,1)を変換する
    return Vector3::Transform(Vector3::Forward, m_rotation);
}

Vector3 TransformComponent::GetRight() const
{
    return Vector3::Transform(Vector3::Right, m_rotation);
}

Vector3 TransformComponent::GetUp() const
{
    return Vector3::Transform(Vector3::Up, m_rotation);
}