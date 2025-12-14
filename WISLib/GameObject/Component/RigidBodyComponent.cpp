#include "pch.h"
#include "RigidBodyComponent.h"

#include "TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"

#include "UserResources.h"



using namespace WIS;
using namespace DirectX;
using namespace SimpleMath;
// コンストラクタ: 親オブジェクトの参照を保持
// ユーザー提供コードに基づき実装
WIS::RigidBodyComponent::RigidBodyComponent()
{
}

RigidBodyComponent::~RigidBodyComponent() {}

void RigidBodyComponent::OnStart(UserResources* userResources)
{

	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	//BasicEffect の作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	//DrawGrid は VertexPositionColor を使うので、頂点カラーを有効にする
	m_basicEffect->SetVertexColorEnabled(true);

	//Input Layout の作成
	// エフェクトからシェーダーの情報を取得して、レイアウトを作成します
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	// VertexPositionColor 用のレイアウトを作成して保持
	device->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);
}

void RigidBodyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	m_velocity = Vector3::UnitY;
}

void WIS::RigidBodyComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	auto font = userResources->GetDebugFont();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = userResources->GetDeviceResources()->GetD3DDevice();
	//void DebugFont::AddString(const wchar_t * string, DirectX::SimpleMath::Vector2 pos, FXMVECTOR color, float scale)

	// プリミティブバッチを取得
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch = userResources->GetPrimitiveBatch();

	auto Trans = GetOwner()->GetComponent<TransformComponent>();
	if (!m_basicEffect) {
		return;
	}

	DirectX::FXMMATRIX value = Trans->GetWorldMatrix();
	m_basicEffect->SetWorld(value);


	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	primitiveBatch->Begin();

	//void XM_CALLCONV DX::DrawRay(PrimitiveBatch<VertexPositionColor>*batch,
	//	FXMVECTOR origin,
	//	FXMVECTOR direction,
	//	bool normalize,
	//	FXMVECTOR color)

	DX::DrawRay(
		primitiveBatch,
		{ 0,0,0 },
		m_velocity,
		false,
		DirectX::Colors::LightGreen);
	

	primitiveBatch->End();
}





