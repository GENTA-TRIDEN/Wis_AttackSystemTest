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
		DirectX::SimpleMath::Vector3 gravityForce(0.0f, -m_gravity * m_mass, 0.0f);
		AddForce(gravityForce);
	}

	//加速度を計算する
	DirectX::SimpleMath::Vector3 acceleration = DirectX::SimpleMath::Vector3::Zero;

	//0じゃないかチェックする
	if (m_mass > 0.0001f)
	{
		//力を計算する
		acceleration = m_force / m_mass;
	}



	// 前のフレームの速度に、加速度を足す
	m_velocity += acceleration * elapsedTime;



	//抵抗を適用させる
	if (m_drag > 0.0f)
	{
		// 係数がマイナスにならないようにClampする
		float dragFactor = 1.0f - (m_drag * elapsedTime);
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
	// 必要なデータがない場合は何もしない
	if (!otherFloor ||!myTrans || !otherModelComp || !otherTrans || otherModelComp->VertexList.empty()) return;

// --- 2. レイ（Ray）の設定 ---
	Vector3 currentPos = myTrans->GetPosition();
	Vector3 rayOrigin = currentPos + Vector3(0.0f, 0.5f, 0.0f);

	// ★修正: 明示的に数値でセットして、念のため正規化（Normalize）を呼ぶ
	Vector3 rayDir = Vector3(0.0f, -1.0f, 0.0f);
	rayDir.Normalize(); // これで確実に長さが 1.0 になります

	// もし万が一、長さが0になってしまった場合の防御（今回のケースではDownなのでありえませんが、習慣として）
	if (rayDir.LengthSquared() == 0.0f)
	{
		rayDir = Vector3(0.0f, -1.0f, 0.0f);
	}
	// 判定結果を保存する変数
	float nearestDist = FLT_MAX; // 最も近い地面までの距離
	bool hitGround = false;      // 有効な地面が見つかったかどうかのフラグ

	// 相手モデルの行列と頂点データ
	Matrix objWorld = otherTrans->GetWorldMatrix();         // GameObjectの位置・回転
	Matrix modelLocal = otherModelComp->GetLocalMatrix(0);  // SetRotateなどで設定した回転

	// 合体させる (順番重要: Local * World)
	Matrix finalWorld = modelLocal * objWorld;


	const auto& indices = otherModelComp->IndexList;
	const auto& vertices = otherModelComp->VertexList;

	// --- 3. 全ポリゴンに対してレイ判定ループ ---
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		if (i + 2 >= indices.size()) break;

		// 3つの頂点を取得してワールド座標に変換
		uint16_t i0 = indices[i];
		uint16_t i1 = indices[i + 1];
		uint16_t i2 = indices[i + 2];

		Vector3 v0 = Vector3::Transform(vertices[i0].position, finalWorld);
		Vector3 v1 = Vector3::Transform(vertices[i1].position, finalWorld);
		Vector3 v2 = Vector3::Transform(vertices[i2].position, finalWorld);

		// レイと三角形の交差判定 (dist には rayOrigin からの距離が入る)
		float dist = 0.0f;
		if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
		{
			//今まで見つけた中で一番近いこと
			//足元付近であること (例: 1.0f以内 = 開始高さ0.5 + 足元の余裕0.5)
			if (dist < nearestDist && dist < 1.0f)
			{
				//角度チェック (急な坂は地面とみなさない)
				Vector3 edge1 = v1 - v0;
				Vector3 edge2 = v2 - v0;
				Vector3 normal = edge1.Cross(edge2);
				normal.Normalize();

				// 上向き成分が0.7より大きい（＝緩やかな坂）場合のみ地面とする
				// これにより、急な坂や壁の上には乗れなくなります
				if (normal.Dot(Vector3::Up) > 0.7f)
				{
					nearestDist = dist;
					hitGround = true;
				}
			}
		}
	}

	// --- 4. 接地処理 (スナップ) ---
	if (hitGround)
	{
		m_isGrounded = true;

		// 地面の高さを計算 (レイの発射地点 - 距離)
		float groundY = rayOrigin.y - nearestDist;

		// キャラクターの位置を更新（高さ合わせ）
		// 現在のX,Z座標はそのままに、Y座標だけ地面の高さにする
		// ※モデルの原点が「足元」にある場合の計算です
		Vector3 newPos = currentPos;
		newPos.y = groundY;

		// ジャンプ中(上昇中)はスナップしないように、落下中または静止中のみ適用
		if (m_velocity.y <= 0)
		{
			myTrans->SetPosition(newPos);
			m_velocity.y = 0; // 落下速度をリセットして止める
		}
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





