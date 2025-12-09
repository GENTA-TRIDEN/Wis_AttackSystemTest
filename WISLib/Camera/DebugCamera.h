#pragma once
#include "CameraBase.h"

namespace WIS
{
	class DebugCamera :public WIS::CameraBase
	{
	private:

		// カメラの距離
		static const float DEFAULT_CAMERA_DISTANCE;

		// 横回転
		float m_yAngle, m_yTmp;

		// 縦回転
		float m_xAngle, m_xTmp;

		// ドラッグされた座標
		int m_x, m_y;

		float m_sx, m_sy;

		//// 生成されたビュー行列
		//DirectX::SimpleMath::Matrix m_view;

		// スクロールフォイール値
		int m_scrollWheelValue;

		//// 視点
		//DirectX::SimpleMath::Vector3 m_eye;

		//// 注視点
		//DirectX::SimpleMath::Vector3 m_target;

		// マウストラッカー
		DirectX::Mouse::ButtonStateTracker m_tracker;

		// スクリーンサイズ
		int m_screenW, m_screenH;



		void Motion(int x, int y);
	public:
		DebugCamera(DirectX::SimpleMath::Vector3 position);
		~DebugCamera();
		//派生オブジェクト特有の動き
		void UniqueInitialize()override;
		void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)override;
		void UniqueRander()override;
		void Uniquefinalize()override;
		//カメラの種類を取得




	};


}

