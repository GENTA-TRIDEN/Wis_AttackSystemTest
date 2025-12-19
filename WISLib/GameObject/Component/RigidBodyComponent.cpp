#include "pch.h"
#include "RigidBodyComponent.h"

#include "TransformComponent.h"
#include "FloorComponent.h"
#include "ModelComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h"



using namespace WIS;
using namespace DirectX;
using namespace SimpleMath;


WIS::RigidBodyComponent::RigidBodyComponent()
	: m_velocity(DirectX::SimpleMath::Vector3::Zero)  // 速度を0に
	, m_force(DirectX::SimpleMath::Vector3::Zero)     // 力を0に
	, m_useGravity(true)                              // 重力を有効にするか（デフォルトtrue）
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

void WIS::RigidBodyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	//TransformComponentのnullCheck
	auto transform = GetOwner()->GetComponent<WIS::TransformComponent>();
	if (!transform) return;



	// F = mg (力 = 質量 * 重力加速度)
	//if (m_useGravity)
	if (m_useGravity)
	{

		
		// Y軸下向きに力を加える
		DirectX::SimpleMath::Vector3 gravityForce(0.0f, -m_value.gravity * m_value.mass, 0.0f);
		AddForce(gravityForce);
	}

	//加速度を計算する
	DirectX::SimpleMath::Vector3 acceleration = DirectX::SimpleMath::Vector3::Zero;

	//0じゃないかチェックする
	if (m_value.mass > 0.0001f)
	{
		//力を計算する
		acceleration = m_force / m_value.mass;
	}



	// 前のフレームの速度に、加速度を足す
	m_velocity += acceleration * elapsedTime;



	//抵抗を適用させる
	if (m_value.drag > 0.0f)
	{
		// 係数がマイナスにならないようにClampする
		float dragFactor = 1.0f - (m_value.drag * elapsedTime);
		if (dragFactor < 0.0f) dragFactor = 0.0f;

		m_velocity *= dragFactor;
	}

	//位置を更新する
	transform->Translate(m_velocity * elapsedTime);

	//力のリセット
	m_force = DirectX::SimpleMath::Vector3::Zero;
}

void WIS::RigidBodyComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
	// --- 1. 必要な情報の取得 ---
	auto myTrans = GetOwner()->GetComponent<TransformComponent>();
	auto otherModelComp = otherObject->GetComponent<ModelComponent>();
	auto otherTrans = otherObject->GetComponent<TransformComponent>();
	auto otherFloor = otherObject->GetComponent<FloorComponent>();

	if (!otherFloor || !myTrans || !otherModelComp || !otherTrans || otherModelComp->VertexList.empty()) return;

	// --- 2. レイ（Ray）の設定 ---
	Vector3 currentPos = myTrans->GetPosition();
	Vector3 rayOrigin = currentPos + Vector3(0.0f, 0.5f, 0.0f);

	Vector3 rayDir = Vector3(0.0f, -1.0f, 0.0f);
	rayDir.Normalize();

	// 判定結果を保存する変数
	float nearestDist = FLT_MAX;
	bool hitAnyPoly = false;     // 何らかのポリゴンに当たったか
	Vector3 nearestNormal = Vector3::Up; // 最も近い面の法線を保存しておく

	// 行列計算
	Matrix objWorld = otherTrans->GetWorldMatrix();
	Matrix modelLocal = otherModelComp->GetLocalMatrix(0);
	Matrix finalWorld = modelLocal * objWorld;

	const auto& indices = otherModelComp->IndexList;
	const auto& vertices = otherModelComp->VertexList;

	// --- 3. 全ポリゴンに対してレイ判定ループ ---
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		if (i + 2 >= indices.size()) break;

		uint16_t i0 = indices[i];
		uint16_t i1 = indices[i + 1];
		uint16_t i2 = indices[i + 2];

		Vector3 v0 = Vector3::Transform(vertices[i0].position, finalWorld);
		Vector3 v1 = Vector3::Transform(vertices[i1].position, finalWorld);
		Vector3 v2 = Vector3::Transform(vertices[i2].position, finalWorld);

		float dist = 0.0f;
		if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
		{
			// 足元付近（1.0f以内）で、かつ今までで一番近い場合
			if (dist < nearestDist && dist < 1.0f)
			{
				// ここでは角度チェックをせず、物理的に当たっている事実だけを記録する
				nearestDist = dist;
				hitAnyPoly = true;

				// 法線を計算して保存しておく
				Vector3 edge1 = v1 - v0;
				Vector3 edge2 = v2 - v0;
				Vector3 normal = edge1.Cross(edge2);
				normal.Normalize();
				nearestNormal = normal;
			}
		}
	}

	// --- 4. 衝突後の処理 ---
	if (hitAnyPoly)
	{
		// 地面の高さ
		float groundY = rayOrigin.y - nearestDist;
		Vector3 newPos = currentPos;
		newPos.y = groundY;

		// 法線の上向き成分を確認
		float upDot = nearestNormal.Dot(Vector3::Up);

		// 緩やかな坂（歩ける地面）の場合: 今まで通りの処理
		// cos(45度) ≒ 0.707 なので、0.7fより大きければ45度未満
		if (upDot > 0.9f)
		{
			m_isGrounded = true;

			// 落下中または静止中のみ高さ合わせ
			if (m_velocity.y <= 0)
			{
				myTrans->SetPosition(newPos);
				m_velocity.y = 0;

			}

		}
		// B. 急な坂（壁）の場合: すり抜け防止処理
		else
		{


			if (m_velocity.y < 0)
			{
				myTrans->SetPosition(newPos);

				// 坂を滑り落ちる表現にしたい場合は velocity.y をリセットしない、
				// または少しだけ減衰させるなどの処理を入れます。
				// ここでは完全に止まらず、ゆっくり滑るように摩擦を入れる例です：
				 m_velocity.y *= 0.9f; 
			}



			// 急な坂なので接地判定は出さない（ジャンプできないようにする）
			m_isGrounded = false;


			//適用する
			Vector3 pushDir = nearestNormal;
			pushDir.y = 0; // 水平成分のみ
			pushDir.Normalize();
			Vector3 pushPos = myTrans->GetPosition();
			pushPos += pushDir * 0.1f; // 少し押し出す
			myTrans->SetPosition(pushPos);
			

			AddImpulse(pushDir * 2);

		}
	}
	else
	{
		// 何も当たらなかったら空中
		m_isGrounded = false;
	}
}

void WIS::RigidBodyComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	auto font = userResources->GetDebugFont();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	// プリミティブバッチを取得
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch = userResources->GetPrimitiveBatch();

	auto Trans = GetOwner()->GetComponent<TransformComponent>();
	if (!m_basicEffect) {
		return;
	}

	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);

	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	primitiveBatch->Begin();



	DirectX::SimpleMath::Vector3 worldPos = Trans->GetPosition();

	DX::DrawRay(
		primitiveBatch,
		worldPos,       
		m_velocity,     
		false,          
		DirectX::Colors::LightGreen);

	
	DirectX::SimpleMath::Vector3 spherePos = worldPos + m_velocity;
	BoundingSphere boundingSphere = BoundingSphere(spherePos, 0.2f);

	DX::Draw(
		primitiveBatch,
		boundingSphere,
		DirectX::Colors::LightGreen);

	primitiveBatch->End();
}





