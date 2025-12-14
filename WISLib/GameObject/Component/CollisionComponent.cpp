#include "pch.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/Common/CollisionManager.h"
#include "UserResources.h"
#include "VertexTypes.h"


using namespace WIS;

bool CollisionComponent::m_IsDrawCollisions = true;


CollisionComponent::CollisionComponent() :
	m_boundingType(WIS::BoundingType::NONE)
{
	//UniqueNumberの初期化
	CollisionUniqueNumber = rand();
	m_IsDrawCollision = true;
}

CollisionComponent::~CollisionComponent()
{
	// 念のためデストラクタでも登録解除を呼ぶ
	OnRelease();
	m_color = DirectX::Colors::Red;
}

void CollisionComponent::AddBoundingOrientedBox(const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Vector3& center)
{
	m_boundingOrientedBox = std::make_unique<DirectX::BoundingOrientedBox>(
		center,
		extents,
		DirectX::SimpleMath::Quaternion::Identity
	);
	m_isActive = true;
	SetBoundingType(WIS::BoundingType::OBB);

	m_boundingType = WIS::BoundingType::OBB;

}

void WIS::CollisionComponent::AddBoundingSphere(float radius, const DirectX::SimpleMath::Vector3& center)
{
	m_boundingSphere = std::make_unique<DirectX::BoundingSphere>(
		center,
		radius);
	m_isActive = true;
	SetBoundingType(WIS::BoundingType::SPHERE);


	m_boundingType = WIS::BoundingType::SPHERE;

}

void WIS::CollisionComponent::AddBoundingBox(const DirectX::SimpleMath::Vector3& extents, const DirectX::SimpleMath::Vector3& center)
{
	m_boundingBox = std::make_unique<DirectX::BoundingBox>(
		center - extents,
		center + extents);
	m_isActive = true;
	SetBoundingType(WIS::BoundingType::AABB);

}

void WIS::CollisionComponent::AddBoundingFrustum(float nearPlane, float farPlane, float rightSlope, float leftSlope, float topSlope, float bottomSlope, const DirectX::SimpleMath::Vector3& center)
{
	m_boundingFrustum = std::make_unique<DirectX::BoundingFrustum>(
		DirectX::SimpleMath::Matrix::CreatePerspectiveOffCenter(
			leftSlope * nearPlane,
			rightSlope * nearPlane,
			bottomSlope * nearPlane,
			topSlope * nearPlane,
			nearPlane,
			farPlane
		)
	);
	m_boundingFrustum->Origin = center;
	m_isActive = true;
	SetBoundingType(WIS::BoundingType::FRUSTUM);
}

void WIS::CollisionComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	//まとめて描画するかどうか
	if (!m_IsDrawCollisions)return;
	//個別で描画するかどうか
	if(!m_IsDrawCollision)return;

	auto font = userResources->GetDebugFont();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = userResources->GetDeviceResources()->GetD3DDevice();
	//void DebugFont::AddString(const wchar_t * string, DirectX::SimpleMath::Vector2 pos, FXMVECTOR color, float scale)

	if (IsHit) {

		font->AddString(L"Hit", { 50, 50 }, DirectX::Colors::Green);
		m_color = DirectX::Colors::Green;
	}
	else
	{
		font->AddString(L"NotHit", { 50, 50 }, DirectX::Colors::Red);
		m_color = DirectX::Colors::Red;
	}
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

	switch (m_boundingType)
	{
	case WIS::BoundingType::OBB:
		// BoundingOrientedBox を取得して描画を指示
		if (m_boundingOrientedBox) { // nullptr チェック
			DX::Draw(
				primitiveBatch,
				*m_boundingOrientedBox.get(),
				m_color); 
		}
		break;

	case WIS::BoundingType::SPHERE:
		// BoundingSphere を取得して描画を指示
		if (m_boundingSphere) { // nullptr チェック
			DX::Draw(
				primitiveBatch,
				*m_boundingSphere.get(),
				m_color);
		}
		break;

	case WIS::BoundingType::AABB:
		// BoundingBox を取得して描画を指示
		if (m_boundingBox) { // nullptr チェック
			DX::Draw(
				primitiveBatch,
				*m_boundingBox.get(),
				m_color);
		}
		break;

	case WIS::BoundingType::FRUSTUM:
		// BoundingFrustum を取得して描画を指示
		if (m_boundingFrustum) { // nullptr チェック
			DX::Draw(
				primitiveBatch,
				*m_boundingFrustum.get(),
				m_color);
		}
		break;

	default:
		break;
	}

	primitiveBatch->End();
	IsHit = false;
}


void CollisionComponent::OnStart(UserResources* userResources)
{
	if (m_pCollisionManager) {
		// GetOwner()が返すshared_ptrをCollisionManagerに渡す
		m_pCollisionManager->ResistCollisionObject(GetOwner());
	}
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

void CollisionComponent::OnPostUpdate(const float /*elapsedTime*/, UserResources* /*userResources*/)
{

	if (m_boundingOrientedBox)
	{
		auto owner = GetOwner();
		if (!owner) return;

		auto transform = owner->GetComponent<TransformComponent>();
		if (!transform) return;

		// 親子関係を考慮したワールド行列を取得
		auto worldMatrix = transform->GetWorldMatrix();

		// ワールド行列からワールド座標系での位置、回転、スケールを分解
		DirectX::SimpleMath::Vector3 worldScale, worldPosition;
		DirectX::SimpleMath::Quaternion worldRotation;
		if (worldMatrix.Decompose(worldScale, worldRotation, worldPosition))
		{
		}
	}
}

// 衝突イベント発生時に呼ばれる
void CollisionComponent::OnCollision(std::shared_ptr<GameObject> otherObject)
{
	// 具体的な処理は、このクラスを継承した PlayerComponent などで実装する
	for (auto func : m_onCollisionCallbacks) {
		func(otherObject);
	}
	IsHit = true;

}

void CollisionComponent::OnRelease()
{
	if (m_pCollisionManager) {
		if (auto owner = GetOwner()) {
			m_pCollisionManager->RemoveCollisionObject(owner);
		}
		m_pCollisionManager = nullptr; // OnRelease後は何もしない
	}

	m_boundingOrientedBox = nullptr;
	m_boundingSphere = nullptr;
	m_boundingBox = nullptr;
	m_boundingFrustum = nullptr;

}