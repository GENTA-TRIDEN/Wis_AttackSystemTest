#include "pch.h"
#include "HPBar.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace WIS
{
	HPBar::HPBar()
		: m_pDR(nullptr)
		, m_offsetY(2.0f)
		, m_barSize(1.5f, 0.2f)
	{
	}

	HPBar::~HPBar()
	{
	}

	void HPBar::Create(DX::DeviceResources* pDR, float offsetY, float width, float height)
	{
		m_pDR = pDR;
		m_offsetY = offsetY;
		m_barSize = Vector2(width, height);

		auto device = pDR->GetD3DDevice();
		auto context = pDR->GetD3DDeviceContext();

		// ステートとバッチの作成
		m_states = std::make_unique<CommonStates>(device);
		m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);

		// エフェクトの作成 (TextureとVertexColorを有効化)
		m_effect = std::make_unique<BasicEffect>(device);
		m_effect->SetTextureEnabled(true);
		m_effect->SetVertexColorEnabled(true);

		// インプットレイアウトの作成
		void const* shaderByteCode;
		size_t byteCodeLength;
		m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		device->CreateInputLayout(
			VertexPositionColorTexture::InputElements,
			VertexPositionColorTexture::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf()
		);


		CreateWICTextureFromFile(device, L"Resources/textures/GF_HPGage.png", nullptr, m_texForeground.ReleaseAndGetAddressOf());
		CreateWICTextureFromFile(device, L"Resources/textures/GF_HPGageBackground.png", nullptr, m_texBackground.ReleaseAndGetAddressOf());
	}

	void HPBar::Render(
		const Vector3& worldPos,
		const Matrix& view,
		const Matrix& proj,
		const Vector3& cameraPos,
		float currentHP,
		float maxHP)
	{
		auto context = m_pDR->GetD3DDeviceContext();


		Vector3 targetPos = worldPos;
		targetPos.y += m_offsetY;

		//billboard計算
		Matrix billboardMat = Matrix::CreateBillboard(targetPos, cameraPos, Vector3::Up);

		// エフェクト設定
		m_effect->SetView(view);
		m_effect->SetProjection(proj);

		m_effect->SetWorld(Matrix::Identity);

		// 4. ステート設定
		context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);

		context->OMSetDepthStencilState(m_states->DepthRead(), 0);
		context->RSSetState(m_states->CullNone());

		context->IASetInputLayout(m_inputLayout.Get());

		// --- 頂点生成 ---
		float w = m_barSize.x;
		float h = m_barSize.y;
		float halfW = w * 0.5f;
		float halfH = h * 0.5f;

		auto TransformVert = [&](Vector3 v) -> Vector3 {
			return Vector3::Transform(v, billboardMat);
			};
		Vector3 tl = Vector3(-halfW, halfH, 0); // 左上
		Vector3 tr = Vector3(halfW, halfH, 0);  // 右上
		Vector3 bl = Vector3(-halfW, -halfH, 0); // 左下
		Vector3 br = Vector3(halfW, -halfH, 0);  // 右下

		VertexPositionColorTexture vBack[4];
		vBack[0] = VertexPositionColorTexture(TransformVert(tl), Colors::White, Vector2(0, 0));
		vBack[1] = VertexPositionColorTexture(TransformVert(tr), Colors::White, Vector2(1, 0));
		vBack[2] = VertexPositionColorTexture(TransformVert(bl), Colors::White, Vector2(0, 1));
		vBack[3] = VertexPositionColorTexture(TransformVert(br), Colors::White, Vector2(1, 1));

		m_batch->Begin();
		m_effect->SetTexture(m_texBackground.Get());
		m_effect->Apply(context);
		m_batch->DrawQuad(vBack[0], vBack[1], vBack[3], vBack[2]);
		m_batch->End();


		float ratio = 1.0f;
		if (maxHP > 0)
		{
			ratio = currentHP / maxHP;
			ratio = std::max(0.0f, std::min(ratio, 1.0f)); // 0~1にクランプ
		}

		// HPがある場合のみ描画
		if (ratio > 0.0f)
		{
			float currentW = w * ratio; // 現在の幅

			//少し手前にずらす
			float zOffset = -0.01f;

			// 左端(-halfW)は固定、右端を計算
			// 左上
			Vector3 f_tl = Vector3(-halfW, halfH, zOffset);
			// 右上 (左端 + 現在の幅)
			Vector3 f_tr = Vector3(-halfW + currentW, halfH, zOffset);
			// 左下
			Vector3 f_bl = Vector3(-halfW, -halfH, zOffset);
			// 右下 (左端 + 現在の幅)
			Vector3 f_br = Vector3(-halfW + currentW, -halfH, zOffset);

			VertexPositionColorTexture vFront[4];
			// UV(0,0)~(1,1)のままにすると、画像が横に縮む演出になります
			vFront[0] = VertexPositionColorTexture(TransformVert(f_tl), Colors::White, Vector2(0, 0));
			vFront[1] = VertexPositionColorTexture(TransformVert(f_tr), Colors::White, Vector2(1, 0));
			vFront[2] = VertexPositionColorTexture(TransformVert(f_bl), Colors::White, Vector2(0, 1));
			vFront[3] = VertexPositionColorTexture(TransformVert(f_br), Colors::White, Vector2(1, 1));

			m_batch->Begin();
			m_effect->SetTexture(m_texForeground.Get());
			m_effect->Apply(context);
			m_batch->DrawQuad(vFront[0], vFront[1], vFront[3], vFront[2]);
			m_batch->End();
		}

	}

}