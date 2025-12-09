#pragma once
//カメラモード
#include "WISLib/Camera/FPSCamera.h"
#include "WISLib/Camera/FreeMoveCamera.h"
#include "WISLib/Camera/VehicleCamera.h"
#include "WISLib/Camera/FixedCamera.h"
#include "WISLib/Camera/TPSCamera.h"

namespace WIS {

	static constexpr float s_DeadZone = 0.5f;
	class CameraBase;
	class CameraManager
	{
	public:


	private:



		std::vector<std::pair<std::unique_ptr<WIS::CameraBase>,int>> m_CameraVec;

		int NowCameraNum;
		int Number;



	public:

		CameraManager();

		static	CameraManager* GetInstance();

		void Initialize();
		void Update(const DirectX::Mouse::State& state);
		void Rander();
		void Finalize();

		//////////////////////
		//ゲッターセッター///
		////////////////////
		//現在のカメラのView行列を取得
		DirectX::SimpleMath::Matrix* GetView()
		{
			return m_CameraVec[NowCameraNum].first->GetView();
		}



		WIS::CameraBase* GetCamera(int handle)
		{
			// std::find_if を使い、ラムダ式で比較条件を指定する
			auto it = std::find_if(m_CameraVec.begin(), m_CameraVec.end(),
				[handle](const std::pair<std::unique_ptr<WIS::CameraBase>, int>& camera) {
					// pair の second（ハンドル）と引数の handle を比較
					return camera.second == handle;
				});

			if (it == m_CameraVec.end())
			{
				return nullptr; // ハンドルが見つからない場合はnullptrを返す
			}

			return it->first.get(); // 見つかったハンドルのカメラを返す
		}

		WIS::CameraBase* GetCamera()
		{
			return m_CameraVec[NowCameraNum].first.get();
		}
		//Handleを取得
		int GetCurrentCameraHandle()
		{
			return m_CameraVec[NowCameraNum].second;

		}

		//////////////////////
		// camera切り替え ///
		////////////////////

		//Newで作ったカメラオブジェクトを
		//Handleを返す
		int AddCamera(const DirectX::SimpleMath::Vector3& pos, const CameraMode& mode,const int Handle);


		int AddCamera(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const CameraMode& mode, const int Handle);

		//種類関係なく次のカメラに移動
		void RequestNextCamera(bool Next=true);

		//現在と同じ種類の次のカメラに移動する
		void RequestNextCameraSameType(bool Next = true);

		//いちばん最初のカメラに移動
		void RequestFirstCamera();

		//タイプ別のいちばん最初のカメラに移動
		void RequestFirstCameraInType(const CameraMode& mode);

		//カメラに移動(インデックス指定)
		//ない場合は0番
		void RequestCamera(int Index);

		//指定タイプのカメラに移動(インデックス指定)
		//ない場合は0番
		void RequestFirstCameraInType(const CameraMode& mode, int Index);

		void RequestCameraByHandle(const int handle);
		//////////////////////
		// camera切り替え ///
		////////////////////
	private:
		//固有のHandleを生成
		int GenerateUniqueHandle(const int handle);

	//シングルトンにするためのunique_ptr
	static std::unique_ptr<CameraManager> s_cameraManager;


	
	};
};