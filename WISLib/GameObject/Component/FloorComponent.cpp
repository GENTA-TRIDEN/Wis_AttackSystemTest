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

float WIS::FloorComponent::GetHeight(DirectX::SimpleMath::Vector3 position)
{
    // 1. 兄弟コンポーネントである ModelComponent を取得
    // (データはここにすべてあるため)
    auto modelComp = GetOwner()->GetComponent<ModelComponent>();
    auto transform = GetOwner()->GetComponent<TransformComponent>();

    // データがない、または空なら計算できない
    if (!modelComp || !transform || modelComp->VertexList.empty())
    {
        return -FLT_MAX;
    }

    // 2. 行列の計算 (RigidBodyでうまくいっているのと同じ計算をする)
    Matrix objWorld = transform->GetWorldMatrix();        // 床オブジェクトの位置・回転
    Matrix modelLocal = modelComp->GetLocalMatrix(0);     // モデル自体の回転
    Matrix finalWorld = modelLocal * objWorld;            // 合体！


    // 3. レイの設定
    // 「調べたい座標(X, Z)」の遥か上空から、真下にレイを落とす
    // Y=100.0f は天井より高ければ何でもOKです
    Vector3 rayOrigin = Vector3(position.x, 100.0f, position.z);
    Vector3 rayDir = Vector3::Down;

    float nearestDist = FLT_MAX;
    bool hit = false;

    // 参照を短く書く
    const auto& vertices = modelComp->VertexList;
    const auto& indices = modelComp->IndexList;

    // 4. 全ポリゴン判定ループ
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        if (i + 2 >= indices.size()) break;

        uint16_t i0 = indices[i];
        uint16_t i1 = indices[i + 1];
        uint16_t i2 = indices[i + 2];

        // ここが重要：finalWorld を使って変換する
        Vector3 v0 = Vector3::Transform(vertices[i0].position, finalWorld);
        Vector3 v1 = Vector3::Transform(vertices[i1].position, finalWorld);
        Vector3 v2 = Vector3::Transform(vertices[i2].position, finalWorld);

        float dist = 0.0f;
        // レイ判定
        if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
        {
            if (dist < nearestDist)
            {
                // 必要であればここで normal.Dot(Up) > 0.7f の角度チェックを入れても良い
                // (床として使える場所だけを返したい場合)

                nearestDist = dist;
                hit = true;
            }
        }
    }

    // 5. 結果を返す
    if (hit)
    {
        // レイの発射地点から距離分下がった位置が「床の高さ」
        return rayOrigin.y - nearestDist;
    }

    return -FLT_MAX;
}

//float WIS::FloorComponent::GetHeight(DirectX::SimpleMath::Vector3 position)
//{
//    // 1. Transformの取得 (床自体の位置・回転・拡大を考慮するため)
//    auto transform = GetOwner()->GetComponent<TransformComponent>();
//    if (!transform)
//        return -FLT_MAX; // エラー値（無限に低い）
//
//    Matrix worldMatrix = transform->GetWorldMatrix();
//
//    // 2. レイの定義
//    // 指定された位置のはるか上空から、真下にレイを飛ばす
//    // Y=1000.0f はゲームのスケールに合わせて調整してください（天井より高い位置）
//    Vector3 rayOrigin = Vector3(position.x, 1.0f, position.z);
//    Vector3 rayDir = Vector3::Down;
//
//    float nearestDist = FLT_MAX;
//    bool hit = false;
//
//    // 3. 全ポリゴンに対して判定
//    for (size_t i = 0; i < IndexList.size(); i += 3)
//    {
//        if (i + 2 >= IndexList.size()) break;
//
//        // インデックス取得
//        uint16_t i0 = IndexList[i];
//        uint16_t i1 = IndexList[i + 1];
//        uint16_t i2 = IndexList[i + 2];
//
//        // 頂点座標をワールド変換
//        Vector3 v0 = Vector3::Transform(VertexList[i0].position, worldMatrix);
//        Vector3 v1 = Vector3::Transform(VertexList[i1].position, worldMatrix);
//        Vector3 v2 = Vector3::Transform(VertexList[i2].position, worldMatrix);
//
//        // レイと三角形の交差判定
//        float dist = 0.0f;
//        if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
//        {
//            if (dist < nearestDist)
//            {
//                nearestDist = dist;
//                hit = true;
//            }
//        }
//    }
//
//    // 4. 結果を返す
//    if (hit)
//    {
//        // レイの発射地点(Y=1000)から距離分下がった場所が高さ
//        return rayOrigin.y - nearestDist;
//    }
//
//    // 床がない場合は「無限に低い場所」として返す（落下扱いにするため）
//    return -FLT_MAX;
//}
