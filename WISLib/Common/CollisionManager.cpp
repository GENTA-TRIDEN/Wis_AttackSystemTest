#include "pch.h"
#include "CollisionManager.h"
#include "DirectXCollision.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"

// --- GetInstance, ResistCollisionObject, RemoveCollisionObject, Update は変更ありません ---

std::unique_ptr<WIS::CollisionManager> WIS::CollisionManager::s_collisionManager = nullptr;

WIS::CollisionManager* WIS::CollisionManager::GetInstance()
{
    if (s_collisionManager == nullptr) {
        s_collisionManager.reset(new CollisionManager);
    }
    return s_collisionManager.get();
}

void WIS::CollisionManager::ResistCollisionObject(std::shared_ptr<GameObject> obj)
{
    if (obj) {
        m_CollisionObjects.push_back(obj);
    }
}

void WIS::CollisionManager::RemoveCollisionObject(std::shared_ptr<GameObject> obj)
{
    m_CollisionObjects.erase(
        std::remove_if(m_CollisionObjects.begin(), m_CollisionObjects.end(),
            [&](const std::weak_ptr<GameObject>& weakObj) {
                auto sharedObj = weakObj.lock();
                return sharedObj && (sharedObj == obj);
            }),
        m_CollisionObjects.end()
    );
}

void WIS::CollisionManager::Update()
{

    CheckCollisionObjects();
}

void WIS::CollisionManager::CheckCollisionObjects()
{
    //破棄されたオブジェクトを除去し、有効なオブジェクトのリストを作成
    m_CollisionObjects.erase(
        std::remove_if(m_CollisionObjects.begin(), m_CollisionObjects.end(),
            [](const std::weak_ptr<GameObject>& obj) {
                return obj.expired();
            }),
        m_CollisionObjects.end()
    );

    std::vector<std::shared_ptr<GameObject>> liveObjects;
    liveObjects.reserve(m_CollisionObjects.size());
    for (const auto& weakObj : m_CollisionObjects) {
        if (auto sharedObj = weakObj.lock()) {
            liveObjects.push_back(sharedObj);
        }
    }

    //オブジェクトを「床」と「動くオブジェクト」にグループ分けする
    std::vector<std::shared_ptr<GameObject>> floorObjects;
    std::vector<std::shared_ptr<GameObject>> movableObjects;

    for (const auto& obj : liveObjects)
    {
        if (auto coll = obj->GetComponent<CollisionComponent>())
        {
            auto tag = coll->GetCollisionTag();
            if (tag == CollisionTag::FLOOR)
            {
                floorObjects.push_back(obj);
            }
            else if (tag == CollisionTag::DYNAMIC)
            {
                movableObjects.push_back(obj);
            }
        }
    }

    // 3. 【専用判定】動くオブジェクト vs 床オブジェクト
    for (const auto& movObj : movableObjects)
    {
        for (const auto& flrObj : floorObjects)
        {
            // 床との当たり判定を行う (CheckHit内部で形状判定)
            if (CheckHit(movObj, flrObj))
            {
                movObj->DispatchCollision(flrObj);
                flrObj->DispatchCollision(movObj);
            }
        }
    }

    //動くオブジェクト同士
    for (size_t i = 0; i < movableObjects.size(); ++i)
    {
        for (size_t j = i + 1; j < movableObjects.size(); ++j)
        {
            if (CheckHit(movableObjects[i], movableObjects[j]))
            {
                movableObjects[i]->DispatchCollision(movableObjects[j]);
                movableObjects[j]->DispatchCollision(movableObjects[i]);
            }
        }
    }
}

bool WIS::CollisionManager::CheckHit(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
{
    auto obj1Comp = obj1->GetComponent<CollisionComponent>();
    auto obj2Comp = obj2->GetComponent<CollisionComponent>();

    if (!obj1Comp || !obj2Comp) return false;


    //CollisionUniqueNumberが同じ場合当たり判定を行わない
	if (obj1Comp->GetCollisionUniqueNumber() == obj2Comp->GetCollisionUniqueNumber()) {
		return false;
	}



    // 静的オブジェクト同士は判定しない
    if (obj1Comp->GetCollisionTag() == CollisionTag::STATIC &&
        obj2Comp->GetCollisionTag() == CollisionTag::STATIC) {
        return false;
    }

    auto obj1Transform = obj1->GetComponent<TransformComponent>();
    auto obj2Transform = obj2->GetComponent<TransformComponent>();

    if (!obj1Transform || !obj2Transform) return false;

    // ワールド行列の取得
    DirectX::SimpleMath::Matrix worldMatrix1 = obj1Transform->GetWorldMatrix();
    DirectX::SimpleMath::Matrix worldMatrix2 = obj2Transform->GetWorldMatrix();

    // -------------------------------------------------------
    // ビット演算による組み合わせ判定
    // -------------------------------------------------------
    BoundingType type1 = obj1Comp->GetBoundingType();
    BoundingType type2 = obj2Comp->GetBoundingType();

    // Enum classのビット演算を行うため int にキャストして計算
    int typeValue1 = static_cast<int>(type1);
    int typeValue2 = static_cast<int>(type2);
    int pairValue = typeValue1 | typeValue2;

    // --- 1. OBB 同士 (OBB | OBB) ---
    if (pairValue == static_cast<int>(BoundingType::OBB))
    {
        auto obb1 = obj1Comp->GetBoundingOrientedBox();
        auto obb2 = obj2Comp->GetBoundingOrientedBox();
        return CheckBoundingCollision(*obb1, worldMatrix1, *obb2, worldMatrix2);
    }
    // --- 2. Sphere 同士 (SPHERE | SPHERE) ---
    else if (pairValue == static_cast<int>(BoundingType::SPHERE))
    {
        auto sphere1 = obj1Comp->GetBoundingSphere();
        auto sphere2 = obj2Comp->GetBoundingSphere();
        return CheckBoundingCollision(*sphere1, worldMatrix1, *sphere2, worldMatrix2);
    }
    // --- 3. AABB 同士 (AABB | AABB) --- 
    else if (pairValue == static_cast<int>(BoundingType::AABB))
    {
        auto aabb1 = obj1Comp->GetBoundingBox();
        auto aabb2 = obj2Comp->GetBoundingBox();
        return CheckBoundingCollision(*aabb1, worldMatrix1, *aabb2, worldMatrix2);
    }
    // --- 4. Sphere と OBB の混合 (SPHERE | OBB) ---
    else if (pairValue == (static_cast<int>(BoundingType::SPHERE) | static_cast<int>(BoundingType::OBB)))
    {
        // ビット積(&)で判定
        bool obj1IsSphere = (typeValue1 & static_cast<int>(BoundingType::SPHERE));
        auto spherePtr = obj1IsSphere ? obj1Comp->GetBoundingSphere() : obj2Comp->GetBoundingSphere();
        auto obbPtr = obj1IsSphere ? obj2Comp->GetBoundingOrientedBox() : obj1Comp->GetBoundingOrientedBox();
        const auto& sphereMat = obj1IsSphere ? worldMatrix1 : worldMatrix2;
        const auto& obbMat = obj1IsSphere ? worldMatrix2 : worldMatrix1;

        return CheckBoundingCollision(*spherePtr, sphereMat, *obbPtr, obbMat);
    }
    // --- 5. Sphere と AABB の混合 (SPHERE | AABB) --- 
    else if (pairValue == (static_cast<int>(BoundingType::SPHERE) | static_cast<int>(BoundingType::AABB)))
    {
        // ビット積(&)で判定
        bool obj1IsSphere = (typeValue1 & static_cast<int>(BoundingType::SPHERE));
        auto spherePtr = obj1IsSphere ? obj1Comp->GetBoundingSphere() : obj2Comp->GetBoundingSphere();
        auto aabbPtr = obj1IsSphere ? obj2Comp->GetBoundingBox() : obj1Comp->GetBoundingBox();
        const auto& sphereMat = obj1IsSphere ? worldMatrix1 : worldMatrix2;
        const auto& aabbMat = obj1IsSphere ? worldMatrix2 : worldMatrix1;

        return CheckBoundingCollision(*spherePtr, sphereMat, *aabbPtr, aabbMat);
    }
    // --- 6. OBB と AABB の混合 (OBB | AABB) --- 
    else if (pairValue == (static_cast<int>(BoundingType::OBB) | static_cast<int>(BoundingType::AABB)))
    {
        // ビット積(&)で判定
        bool obj1IsOBB = (typeValue1 & static_cast<int>(BoundingType::OBB));
        auto obbPtr = obj1IsOBB ? obj1Comp->GetBoundingOrientedBox() : obj2Comp->GetBoundingOrientedBox();
        auto aabbPtr = obj1IsOBB ? obj2Comp->GetBoundingBox() : obj1Comp->GetBoundingBox();
        const auto& obbMat = obj1IsOBB ? worldMatrix1 : worldMatrix2;
        const auto& aabbMat = obj1IsOBB ? worldMatrix2 : worldMatrix1;

        return CheckBoundingCollision(*obbPtr, obbMat, *aabbPtr, aabbMat);
    }

    // 対応していない組み合わせ
    return false;
}