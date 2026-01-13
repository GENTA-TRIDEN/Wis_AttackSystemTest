#pragma once
#include "WISLib/GameObject/ObjectManager.h"
#include "InGameObject/Archetype/PartsArchetype.h"
namespace WIS {
    // リクエスト内容の定義
    struct ItemSpawnRequest {
        WIS::ItemKinds kind;
        DirectX::SimpleMath::Vector3 position;
        std::string baseName;
        DirectX::SimpleMath::Vector3 Impact;
    };

    class ItemRequestManager {
    public:
        // どこからでも呼べるようにリクエストを追加
        static void RequestDrop(WIS::ItemKinds kind, DirectX::SimpleMath::Vector3 pos, const std::string& name = "DroppedItem", DirectX::SimpleMath::Vector3 impact = {0,0,0}) {
            m_requests.push_back({ kind, pos, name,impact });
        }

        // Updateなどで一括生成する関数
        static void ProcessRequests(ObjectManager* pOM, UserResources* pUR) {
            if (m_requests.empty()) return;

            PartsArchetype archetype;
            std::vector<std::string> dummy;

            for (const auto& req : m_requests) {
                // ここで実際のArchetypeを呼び出す
                archetype.Create(pOM, pUR, req.baseName, 0, dummy, req.kind, req.position,req.Impact);
            }

            // 処理が終わったのでリクエストを空にする
            m_requests.clear();
        }

    private:
        static std::vector<ItemSpawnRequest> m_requests;
    };
}