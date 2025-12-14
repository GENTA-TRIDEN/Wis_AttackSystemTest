#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <memory>
#include <vector>
#include <string>

namespace WIS {
	// 前方宣言
	class GameObject;
	class WeaponComponent;

	class RigidBodyComponent : public WIS::ComponentBase
	{
	private:

		//現在の移動
		DirectX::SimpleMath::Vector3 m_velocity;

		//加算される力
		DirectX::SimpleMath::Vector3 m_force;
		float m_gravity = 9.8f;

		//質量
		float m_mass = 1.0f;

		//抵抗
		float m_drag = 0.0f;

		bool m_useGravity;

		// デバッグ描画用のBasicEffect
		std::unique_ptr<DirectX::BasicEffect> m_basicEffect = nullptr;
		//入力レイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	public:
		RigidBodyComponent();
		~RigidBodyComponent();

		//--- ComponentBaseのオーバーライド関数 ---
		void OnStart(UserResources* userResources) override;
		void OnUpdate(const float elapsedTime, UserResources* userResources) override;

		void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)override;
		// 衝突処理は親武器のWeaponComponentに任せることが多いため、ここではロジックを持たない
		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;
		//力を加算する
		void AddForce(DirectX::SimpleMath::Vector3 addForce);
		//直接Velocityを指定する
		void ChangeVelocity(DirectX::SimpleMath::Vector3 NewVelocity);


		//瞬間的な力を掛ける
		void AddImpulse(DirectX::SimpleMath::Vector3 impulse);


	};
}