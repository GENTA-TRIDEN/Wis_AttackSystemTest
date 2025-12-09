#pragma once
#include "CameraBase.h"

namespace WIS
{
	class TPSCamera :public WIS::CameraBase
	{
	private:

		float m_yaw;   // 水平方向（左右）の回転角度
		float m_pitch; // 垂直方向（上下）の回転角度
		DirectX::SimpleMath::Matrix m_transformMatrix; // カメラの変換行列
	public:
		TPSCamera(DirectX::SimpleMath::Vector3 position);
		~TPSCamera();
		//派生オブジェクト特有の動き
		void UniqueInitialize()override;
		void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)override;
		void UniqueRander()override;
		void Uniquefinalize()override;
		//カメラの種類を取得



		float GetYaw() const { return m_yaw; }


	};


}

