#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include "WISLib/Camera/CameraManager.h"

namespace WIS {
    class PlayerComponent : public WIS::ComponentBase
    {

    private :
		DirectX::SimpleMath::Vector3 m_moveDirection;

		float m_moveSpeed = 5.0f; // 移動速度

        
        CameraManager* p_cameraManager = CameraManager::GetInstance();


        //@TODO:アイテムを保持する奴を作ってもいいかも
        
		//ItemListのなかのItemをそれぞれ何個持っているかの配列
		std::unordered_map<WIS::ItemKinds, int> m_itemCountMap;
        

    public:
		PlayerComponent();
		~PlayerComponent();

        void OnStart(UserResources* userResources) override;
        void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj) override;
        void OnRelease() override;


		void AddItem(WIS::ItemKinds itemType) {
			m_itemCountMap[itemType]++;
		}

        int GetItemCount(WIS::ItemKinds itemType) {
			if (m_itemCountMap.find(itemType) == m_itemCountMap.end()) {
				return 0;
			}
			return m_itemCountMap[itemType];
        }

        void SetItemCount(WIS::ItemKinds itemType, int ItemCount) {

            m_itemCountMap[itemType] = ItemCount;
        }

    };
}

