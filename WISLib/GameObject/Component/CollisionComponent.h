#pragma once
#include "ComponentBase.h"
#include "WISLib/Common/CollisionManager.h"
#include <memory> 
#include <wrl/client.h> 

// 前方宣言
namespace WIS {
	class GameObject;
}

namespace WIS {
	class CollisionComponent :public WIS::ComponentBase
	{
	private:



		// BoundingOrientedBox (OBB)
		std::unique_ptr<DirectX::BoundingOrientedBox> m_boundingOrientedBox = nullptr;
		std::unique_ptr<DirectX::BoundingSphere> m_boundingSphere = nullptr;
		std::unique_ptr<DirectX::BoundingBox> m_boundingBox = nullptr;
		std::unique_ptr<DirectX::BoundingFrustum> m_boundingFrustum = nullptr;

		// デバッグ描画用のBasicEffect
		std::unique_ptr<DirectX::BasicEffect> m_basicEffect = nullptr;


		// CollisionManagerのインスタンス
		CollisionManager* m_pCollisionManager = CollisionManager::GetInstance();

		// コリジョンタグ
		CollisionTag m_CollisionTag{ CollisionTag::DYNAMIC };

		WIS::BoundingType m_boundingType{ WIS::BoundingType::OBB };

		bool m_isActive = true;


		//当たった時に実行される関数を格納
		std::vector<std::function<void(std::shared_ptr<GameObject>)>> m_onCollisionCallbacks;


		// ★追加: 入力レイアウト (これがないと描画できません)
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		//debugびょうがのcolorを保持
		DirectX::XMVECTOR m_color;

		//この数値が同じ場合当たり判定を行わない
		int CollisionUniqueNumber = -1;


		bool IsHit;
	public:
		CollisionComponent();
		~CollisionComponent();

		void OnStart(UserResources* userResources) override;
		void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;

		//  OnCollisionの引数を shared_ptr に変更
		void OnCollision(std::shared_ptr<GameObject> otherObject) override;
		// コールバック関数の登録とクリア

		//コールバック関数の登録
		void SetOnCollisionCallback(std::function<void(std::shared_ptr<GameObject>)> func) {
			m_onCollisionCallbacks.push_back(func);
		}
		//コールバック関数のクリア
		void ClearOnCollisionCallbacks() {
			m_onCollisionCallbacks.clear();
		}

		void OnRelease() override;

		// OBBを追加するための関数
		void AddBoundingOrientedBox(const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Vector3& center = { 0.0f, 0.0f, 0.0f });
		// OBBを取得するためのゲッター
		DirectX::BoundingOrientedBox* GetBoundingOrientedBox() { return m_boundingOrientedBox.get(); }
		// 球体のバウンディングを追加/取得する関数
		void AddBoundingSphere(float radius, const DirectX::SimpleMath::Vector3& center = { 0.0f, 0.0f, 0.0f });
		DirectX::BoundingSphere* GetBoundingSphere() { return m_boundingSphere.get(); }
		// AABBを追加/取得する関数
		void AddBoundingBox(const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Vector3& center = { 0.0f, 0.0f, 0.0f });
		DirectX::BoundingBox* GetBoundingBox() { return m_boundingBox.get(); }
		// Frustumを追加/取得する関数
		void AddBoundingFrustum(float nearPlane, float farPlane, float rightSlope, float leftSlope, float topSlope, float bottomSlope, const DirectX::SimpleMath::Vector3& center = { 0.0f, 0.0f, 0.0f });
		DirectX::BoundingFrustum* GetBoundingFrustum() { return m_boundingFrustum.get(); }


		// バウンディングタイプのゲッター/セッター
		void SetBoundingType(BoundingType type) { m_boundingType = type; }
		BoundingType GetBoundingType() { return m_boundingType; }


		// 有効フラグのゲッター/セッター
		bool GetActive() { return m_isActive; }
		void SetActive(bool isActive) { m_isActive = isActive; }

		// コリジョンタグのゲッター/セッター
		void SetCollisionTag(CollisionTag tag) { m_CollisionTag = tag; }
		CollisionTag GetCollisionTag() { return m_CollisionTag; }

		// 今回使わない関数
		void OnPreUpdate(const float, UserResources*) override {}
		void OnUpdate(const float, UserResources*) override {}

		void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)override;



		void SetDebugColor(DirectX::FXMVECTOR color) {
			m_color = color;
		}

		int GetCollisionUniqueNumber() {
			return CollisionUniqueNumber;
		}
	};
}