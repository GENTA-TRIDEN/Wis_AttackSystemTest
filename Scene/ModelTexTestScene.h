#pragma once

#include "ItoLib/SceneManager.h"
#include "../UserResources.h"

#include "WISLib/Camera/CameraManager.h",
#include "WISLib/UserInterface/HitEffect.h"


#include "WISLib/UserInterface/Button.h"


class ModelTexTestScene :public Ito::Scene<UserResources>
{
public:
	ModelTexTestScene();
	~ModelTexTestScene();
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



	//描画結果を保存するテクスチャのリソース
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTargetTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SAVEshaderResourceView;

	//深度バッファ (3D描画には必須なので、テクスチャ用にも個別に用意する)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	//テクスチャを画面に描画するためのバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;


	//WIS::BlueprintManager* m_blueprintManager; // BlueprintManagerを追加
	WIS::ObjectManager* m_objectManager;
	WIS::CameraManager* p_cameraManager = WIS::CameraManager::GetInstance();

	//ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	std::shared_ptr<WIS::GameObject> m_skyDome;

	std::string m_floorText = "Floor";
	std::vector <std::string> m_itemNames;

	int m_cameraHandle;
	int m_FloorModelHandle;


	int m_DebugCountUp = 0;
	const int m_DebugCountMax = 60;


	//WeaponComponentテスト用の変数
	float m_WeaponAttackTime = 0;
	int m_WeaponAttackTimeMax = 60;

	//Attack中かどうか
	bool m_isAttack = false;

	std::unique_ptr<WIS::HitEffect> m_effect;

	std::unique_ptr<kHorikawa::UserInterface> m_GEARSilhouette;
	std::unique_ptr<kHorikawa::UserInterface> m_BOMBSilhouette;

	std::unique_ptr<kHorikawa::UserInterface> m_HPGreen;
	std::unique_ptr<kHorikawa::UserInterface> m_HPRed;

private:
	void Attack();


	std::string ItemGenerate(DirectX::SimpleMath::Vector3 Pos, WIS::ItemKinds kinds);
};

