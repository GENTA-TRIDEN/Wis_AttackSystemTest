//--------------------------------------------------------------------------------------
// File: Particle.h
//
// パーティクルクラス
//
//-------------------------------------------------------------------------------------

#pragma once

#include "StepTimer.h"
#include <DeviceResources.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <list>
namespace kHorikawa {
	class ParticleUtility;
}

namespace WIS
{
	class HitEffect
	{
	public:
		//	データ受け渡し用コンスタントバッファ(送信側)
		struct ConstBuffer
		{
			DirectX::SimpleMath::Matrix		matWorld;
			DirectX::SimpleMath::Matrix		matView;
			DirectX::SimpleMath::Matrix		matProj;
			DirectX::SimpleMath::Vector4	Diffuse;
		};
	private:
		//	変数
		DX::DeviceResources* m_pDR;

		//	経過時間記録用
		float m_timer;

		Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

		//	入力レイアウト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		//	プリミティブバッチ
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
		//	コモンステート
		std::unique_ptr<DirectX::CommonStates> m_states;
		//	テクスチャハンドル
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
		//	！New！
		//	頂点情報のリスト
		std::vector<DirectX::VertexPositionColorTexture> m_vertices;
		//	パーティクルに使う情報を保存するためのクラスオブジェクト用リスト
		std::list<kHorikawa::ParticleUtility> m_particleUtility;
		//	カメラに関する情報
		DirectX::SimpleMath::Vector3 m_cameraPosition;
		DirectX::SimpleMath::Vector3 m_cameraTarget;

		//	頂点シェーダ
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		//	ピクセルシェーダ
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		//	ジオメトリシェーダ
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

		DirectX::SimpleMath::Matrix m_world;
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;

		DirectX::SimpleMath::Matrix m_billboard;

	public:
		//	関数
		static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

		HitEffect();
		~HitEffect();

		void LoadTexture(const wchar_t* path);

		void Create(DX::DeviceResources* pDR);

		void Update(const float elapsedTime);

		void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

		void CreateBillboard(
			DirectX::SimpleMath::Vector3 target,
			DirectX::SimpleMath::Vector3 eye,
			DirectX::SimpleMath::Vector3 up);

		void hit(DirectX::SimpleMath::Vector3 pos);
	private:

		void CreateShader();
	};
}