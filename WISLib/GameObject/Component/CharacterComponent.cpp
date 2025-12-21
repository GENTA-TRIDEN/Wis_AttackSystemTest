#include "pch.h"
#include "CharacterComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"

#include "UserResources.h"

#include "WISLib/GameObject/GameObject.h"

WIS::CharacterComponent::CharacterComponent()
{
}

WIS::CharacterComponent::~CharacterComponent()
{
}

void WIS::CharacterComponent::OnStart(UserResources* userResources)
{
	HP = 100;
	MaxHP = 100;
	isInvincible = false;
	invincibleTimer = 0.0f;
	auto device = userResources->GetDeviceResources()->GetD3DDevice();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();

	//ベーシックエフェクトの作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	//ライティングOFF
	m_basicEffect->SetLightingEnabled(false);
	//頂点カラーOFF
	m_basicEffect->SetVertexColorEnabled(false);
	//テクスチャON
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	//プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);

	//影テクスチャの読み込み
	DX::ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile(
			device,
			L"Resources/Textures/Shadow.dds",
			nullptr,
			m_shadowTexture.ReleaseAndGetAddressOf()
		)
	);


	//深度ステンシルステートの作成

	D3D11_DEPTH_STENCIL_DESC desc = {}; //デフォルトで初期化

	//床の設定
	//深度テスト
	desc.DepthEnable = TRUE;									//深度テストをする
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			//深度バッファに書き込みをする
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				//深度値以下なら更新する
	//ステンシルテスト
	desc.StencilEnable = TRUE;									//ステンシルテストをする
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		//初期値(0xff)
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	//初期値(0xff)

	//表面
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//テスト結果が０ならば
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;	//１にする（表示する）
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//そのまま
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//そのまま

	desc.BackFace = desc.FrontFace;								//裏面も表と同じ

	////深度ステンシルステートの作成
	//device->CreateDepthStencilState(&desc, m_depthStencilState_Floor.ReleaseAndGetAddressOf());

	//影の設定
	//深度テスト
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;			//深度バッファに書き込みをする

	//ステンシルテスト
	// ★修正箇所: 床の設定(TRUE)が残っていると描画されないため、影側で無効にする
	desc.StencilEnable = FALSE;

	//表面
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//１ならばそのまま表示

	desc.BackFace = desc.FrontFace;								//裏面も表と同じ


	//深度ステンシルステートの作成
	device->CreateDepthStencilState(&desc, m_depthStencilState_Shadow.ReleaseAndGetAddressOf());


}

void WIS::CharacterComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	invincibleTimer += elapsedTime;

	if (isInvincible && invincibleTimer >= invincibleTime) {
		isInvincible = false;
		invincibleTimer = 0.0f;
	}
	auto coll = GetOwner()->GetComponent<WIS::CollisionComponent>();
	if (isInvincible) {
		coll->SetDebugColor(DirectX::Colors::Yellow);
	}
	else
	{
		coll->SetDebugColor(DirectX::Colors::Blue);
	}
}

void WIS::CharacterComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	auto deviceResources = userResources->GetDeviceResources();
	auto context = deviceResources->GetD3DDeviceContext();
	auto states = userResources->GetCommonStates();
	DirectX::SimpleMath::Vector3 pos = GetOwner()->GetComponent<WIS::TransformComponent>()->GetPosition();



	DrawShadow(context, states, pos, 2.0f, view, proj);
}

void WIS::CharacterComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{

}

void WIS::CharacterComponent::DecreaseHP(int damage)
{
	HP -= damage;
	if (HP < 0)
	{
		HP = 0;
		this->GetOwner()->SetActive(false);
	}
}

void WIS::CharacterComponent::IncreaseHP(int heal)
{

	HP += heal;

}

void WIS::CharacterComponent::TakeAttack(int damage)
{
	if (!isInvincible) {
		DecreaseHP(damage);
		isInvincible = true;
		invincibleTimer = 0.0f;
	}
}


void WIS::CharacterComponent::DrawShadow(ID3D11DeviceContext* context, DirectX::CommonStates* states, DirectX::SimpleMath::Vector3 position, float Radius,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& proj)
{
	//エフェクトの設定&適応
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->SetTexture(m_shadowTexture.Get());
	m_basicEffect->Apply(context);

	//入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	//テクスチャサンプラー
	ID3D11SamplerState* sampler[] = { states->LinearClamp() };
	context->PSSetSamplers(0, 1, sampler);

	//アルファブレンド
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0XFFFFFFFF);

	//深度バッファ
	context->OMSetDepthStencilState(m_depthStencilState_Shadow.Get(), 1);

	DirectX::VertexPositionTexture vertexes[] =
	{
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(0.0f,0.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(1.0f,0.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(0.0f,1.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(1.0f,1.0f))
	};

	uint16_t indexes[] = { 2,3,1,2,1,0 };

	position.y = m_floorHeight;

	float lift = +0.2f;

	// Y成分を 0.0f から lift に変更
	vertexes[0].position = DirectX::SimpleMath::Vector3(-Radius, lift, -Radius) + position;
	vertexes[1].position = DirectX::SimpleMath::Vector3(Radius, lift, -Radius) + position;
	vertexes[2].position = DirectX::SimpleMath::Vector3(-Radius, lift, Radius) + position;
	vertexes[3].position = DirectX::SimpleMath::Vector3(Radius, lift, Radius) + position;


	//影の描画
	m_primitiveBatch->Begin();

	m_primitiveBatch->DrawIndexed(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		indexes, _countof(indexes),
		vertexes, _countof(vertexes)
	);

	m_primitiveBatch->End();

}