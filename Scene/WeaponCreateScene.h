#pragma once

#include "ItoLib/SceneManager.h"
#include "../UserResources.h"

#include "WISLib/Camera/CameraManager.h"

#include "WISLib/UserInterface/Button.h"

#include "InGameObject/Archetype/WeapoArchetype.h"


class WeaponCreateScene :public Ito::Scene<UserResources>
{
public:
	WeaponCreateScene();
	~WeaponCreateScene();
	// 初期化
	void Initialize() override;
	// 更新
	void Update(float elapsedTime) override;
	// 描画
	void Render() override;
	// 終了処理
	void Finalize() override;
	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;
	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;
	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;


private:

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;
	// view行列
	DirectX::SimpleMath::Matrix m_view;
	// 共通ステート
	std::unique_ptr<DirectX::CommonStates> m_commonStates;

	// 入力レイアウトへのポインタ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//WIS::BlueprintManager* m_blueprintManager; // BlueprintManagerを追加
	WIS::ObjectManager* m_objectManager;
	WIS::CameraManager* p_cameraManager = WIS::CameraManager::GetInstance();

	//ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;


	int m_cameraHandle;
	//もとのSceneのカメラのHandle
	int m_ReturnCameraHandle;

	std::unique_ptr <WIS::Button> m_button;
	std::unique_ptr <WIS::Button> m_BombButton;

	std::unique_ptr<WIS::Button> m_UpButton;
	std::unique_ptr<WIS::Button> m_DownButton;
	std::unique_ptr<WIS::Button> m_RightButton;
	std::unique_ptr<WIS::Button> m_LeftButton;


	std::unique_ptr<WIS::Button> m_SelectButton;

	int clickCount = 0;
	int WeaponPartsModelHandle = -1;

	std::vector<std::string> WeaponPartsName;
	bool isClick = false;

	const std::string m_weaponText = "Weapon";
	const std::string m_weaponPartsText = "WeaponParts";
	const std::string m_playerText = "Player";

	//現在選択されているPartsの名前;
	int m_currentWeaponPartsNameIndex;
	int m_WeaponModelHandle = -1;


private:
	//SelectしているWeaponPartsを設定する

	void ChangeNextWeaponParts();




	WIS::WeapoArchetype m_weaponArchetype;

	DirectX::SimpleMath::Vector3 WeaponPos;
	DirectX::SimpleMath::Matrix WeaponMat;

private:
	int m_DebugCountUp = 0;
	const int m_DebugCountMax = 60;

};

