#pragma once

#include "ItoLib/SceneManager.h"
#include "../UserResources.h"

#include "WISLib/Camera/CameraManager.h"

class RigidBodyScene :public Ito::Scene<UserResources>
{
public:
	RigidBodyScene();
	~RigidBodyScene();
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


	std::string m_PlayerText = "Player";
	std::string m_EnemyText = "Enemy";


	std::string m_weaponName = "Weapon";
	std::string m_floorText = "Floor";
	std::vector <std::string> m_itemNames;




	int m_cameraHandle = -1;

	float m_PlayerAttackLength = 1;


	int m_DebugCountUp = 0;
	const int m_DebugCountMax = 60;


	//WeaponComponentテスト用の変数
	float m_WeaponAttackTime = 0;
	int m_WeaponAttackTimeMax = 60;

	//Attack中かどうか
	bool m_isAttack = false;
private:
	void Attack();


	std::string ItemGenerate(DirectX::SimpleMath::Vector3 Pos, WIS::ItemKinds kinds);
};

