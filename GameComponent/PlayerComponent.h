#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include "WISLib/Camera/CameraManager.h"
#include <string>

namespace WIS {
	class PlayerComponent : public WIS::ComponentBase
	{

	private:
		DirectX::SimpleMath::Vector3 m_moveDirection;

		float m_moveSpeed = 15.0f;

		CameraManager* p_cameraManager = CameraManager::GetInstance();

		std::unordered_map<WIS::ItemKinds, int> m_itemCountMap;

		std::string m_weaponName;
		bool m_isAttack = false;
		float m_WeaponAttackTime = 0.0f;

		// •Ší‚Ì‰Šú‰ñ“]i‚¿•ûj‚ğ•Û‘¶‚·‚é•Ï”
		DirectX::SimpleMath::Quaternion m_initialWeaponRotation;
		bool m_hasCapturedRotation = false;

		bool m_IsLeftToRight = true;

		float m_nowRot;
		//‰ñ“]‘¬“x‚Ì”{—¦
		float m_rotSpeedMag =1.0f;
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

		void SetWeaponName(const std::string& name) {
			m_weaponName = name;
		}

	};
}