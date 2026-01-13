#include "pch.h"
#include "FloorComponent.h"
#include "CollisionComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"


#include "WISLib/GameObject/GameObject.h"

using namespace DirectX;
using namespace SimpleMath;
WIS::FloorComponent::FloorComponent()
{
}

WIS::FloorComponent::~FloorComponent()
{
}

void WIS::FloorComponent::OnStart(UserResources* userResources)
{
	auto model=GetOwner()->GetComponent<WIS::ModelComponent>();
	
}

void WIS::FloorComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{

}

void WIS::FloorComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{

}

void WIS::FloorComponent::SetModel()
{
    m_HaveModel = true;

}

//リジッドボディと同様に計算
float WIS::FloorComponent::GetHeight(DirectX::SimpleMath::Vector3 position)
{

    auto transform = GetOwner()->GetComponent<TransformComponent>();
    auto modelComp = GetOwner()->GetComponent<ModelComponent>(); // モデルも取得

	// Safety check
    if (!transform || !modelComp)
    {
        return -FLT_MAX;
    }

    Matrix objWorld = transform->GetWorldMatrix();


    Matrix modelLocal = modelComp->GetLocalMatrix(0);


    Matrix finalWorld = modelLocal * objWorld;



    Vector3 rayOrigin = Vector3(position.x, 100.0f, position.z);
    Vector3 rayDir = Vector3::Down;

    float nearestDist = FLT_MAX;
    bool hit = false;


    const std::vector<DirectX::VertexPositionColor>* pVertices = &modelComp->VertexList;
    const std::vector<uint16_t>* pIndices = &modelComp->IndexList;


    for (size_t i = 0; i < pIndices->size(); i += 3)
    {
        if (i + 2 >= pIndices->size()) break;

        uint16_t i0 = pIndices->at(i);
        uint16_t i1 = pIndices->at(i + 1);
        uint16_t i2 = pIndices->at(i + 2);

        // finalWorld (Scale込み) を使って変換
        Vector3 v0 = Vector3::Transform(pVertices->at(i0).position, finalWorld);
        Vector3 v1 = Vector3::Transform(pVertices->at(i1).position, finalWorld);
        Vector3 v2 = Vector3::Transform(pVertices->at(i2).position, finalWorld);

        float dist = 0.0f;
        // レイ判定
        if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
        {
            if (dist < nearestDist)
            {
                nearestDist = dist;
                hit = true;
            }
        }
    }

    if (hit)
    {
        return rayOrigin.y - nearestDist;
    }

    return -FLT_MAX;
}
