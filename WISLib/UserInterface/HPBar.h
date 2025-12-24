#pragma once
#include <memory>
#include <SimpleMath.h>
#include <CommonStates.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include <WICTextureLoader.h>
#include "DeviceResources.h"

namespace WIS
{
	class HPBar
	{
	private:
		// デバイスリソースへの参照
		DX::DeviceResources* m_pDR;

		// 描画用バッチとステート
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
		std::unique_ptr<DirectX::CommonStates> m_states;

		// シェーダー（標準のBasicEffectを使用）
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		// テクスチャ
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texBackground; // 赤（背景）
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texForeground; // 緑（前面）

		// バーの設定
		DirectX::SimpleMath::Vector2 m_barSize; // バーのワールド空間でのサイズ（幅、高さ）
		float m_offsetY; // 頭上のオフセット

	public:
		HPBar();
		~HPBar();

		void Create(DX::DeviceResources* pDR, float offsetY = 2.0f, float width = 1.5f, float height = 0.2f);


		void Render(
			const DirectX::SimpleMath::Vector3& worldPos,
			const DirectX::SimpleMath::Matrix& view,
			const DirectX::SimpleMath::Matrix& proj,
			const DirectX::SimpleMath::Vector3& cameraPos,
			float currentHP,
			float maxHP
		);


	};
}