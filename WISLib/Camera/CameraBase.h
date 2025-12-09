#pragma once
#include "SimpleMath.h"
#include "WISLib/WISLibTags.h"

namespace WIS {

	class CameraBase
	{
	public:
		CameraBase(DirectX::SimpleMath::Vector3 position, WIS::CameraMode type);
		//? 仮想デストラクタに変更?
		virtual ~CameraBase();

		void Initialize();
		void Update(const DirectX::Mouse::State& state);
		void Rander();
		void Finalize();

		virtual void UniqueInitialize() = 0;
		virtual void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state) = 0;
		virtual void UniqueRander() = 0;
		virtual void Uniquefinalize() = 0;


		WIS::CameraMode GetType() { return m_type; }


		DirectX::SimpleMath::Matrix* GetView() { return &m_view; }
		void SetView(const DirectX::SimpleMath::Matrix view) { m_view = view; }
		void RotateTarget(const DirectX::SimpleMath::Matrix& rot);

		void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
		DirectX::SimpleMath::Vector3 GetDirection() { return m_target - m_position; }
		DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
		DirectX::SimpleMath::Vector3 GetTarget() { return m_target; }
		//VIEW行列を取得


	protected:
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Vector3 m_position;
		DirectX::SimpleMath::Vector3 m_target;
		WIS::CameraMode m_type;
	};
}