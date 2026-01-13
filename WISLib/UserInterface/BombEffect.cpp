//--------------------------------------------------------------------------------------
// File: Particle.cpp
//
// パーティクルクラス
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "BombEffect.h"

#include "BinaryFile.h"
#include "DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <algorithm>
#include <random>
#include "ParticleUtility.h"


/// <summary>
/// インプットレイアウト
/// </summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> WIS::BombEffect::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/// <summary>
/// コンストラクタ
/// </summary>
WIS::BombEffect::BombEffect()
	: m_pDR(nullptr)
	, m_timer(0.0f)
{
}

/// <summary>
/// デストラクタ
/// </summary>
WIS::BombEffect::~BombEffect()
{
}

/// <summary>
/// テクスチャリソース読み込み関数
/// </summary>
/// <param name="path">相対パス(Resources/Textures/・・・.pngなど）</param>
void WIS::BombEffect::LoadTexture(const wchar_t* path)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::CreateWICTextureFromFile(m_pDR->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());

	m_texture.push_back(texture);
}

/// <summary>
/// 生成関数
/// </summary>
/// <param name="pDR">ユーザーリソース等から持ってくる</param>
void WIS::BombEffect::Create(DX::DeviceResources* pDR)
{
	m_pDR = pDR;
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	シェーダーの作成
	CreateShader();

	//	画像の読み込み
	LoadTexture(L"Resources/Textures/GameStart.png");
	LoadTexture(L"Resources/Textures/ReturnPush.png");

	//	プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());

	m_states = std::make_unique<DirectX::CommonStates>(device);

}

/// <summary>
/// 更新関数
/// </summary>
/// <param name="timer">Game等からStepTimerを受け取る</param>
void WIS::BombEffect::Update(const float elapsedTime)
{
	//	2.0秒ごとに1つパーティクルを生成
	m_timer += elapsedTime;
	if (m_timer <= 0.2f)
	{
		//	タイマーのリセット
		m_timer = 0.0f;
	}


	//	timerを渡してm_effectの更新処理を行う
	for (std::list<kHorikawa::ParticleUtility>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{

		//	更新結果の戻り値（true / false）をチェック
		if (!(ite)->Update(elapsedTime))
		{

			//	falseが返ってきたら、消す
			ite = m_particleUtility.erase(ite);

			if (ite == m_particleUtility.end())
			{

				//	最後のオブジェクトを消したので、ループ終了
				break;
			}
		}
	}
}

/// <summary>
/// Shader作成部分だけ分離した関数
/// </summary>
void WIS::BombEffect::CreateShader()
{
	ID3D11Device1* device = m_pDR->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	std::unique_ptr<kHorikawa::BinaryFile> VSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	std::unique_ptr<kHorikawa::BinaryFile> PSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/BombPS.cso");
	std::unique_ptr<kHorikawa::BinaryFile> GSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/BombGS.cso");

	//	インプットレイアウトの作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		VSData->GetData(), VSData->GetSize(),
		m_inputLayout.GetAddressOf());

	//	頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData->GetData(), VSData->GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData->GetData(), PSData->GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData->GetData(), GSData->GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

/// <summary>
/// 描画関数
/// </summary>
/// <param name="view">ビュー行列</param>
/// <param name="proj">射影行列</param>
void WIS::BombEffect::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();
	m_world = m_billboard;
	//	ビルボード設定時にもらったカメラ情報から、視線ベクトルを計算する
	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;

	//	視線ベクトルは正規化しておく
	cameraDir.Normalize();

	//	パーティクル情報を、カメラからの距離順でソートする
	m_particleUtility.sort(
		//	ソート処理の基準を示す関数については、ラムダ式で指定する
		[&](kHorikawa::ParticleUtility lhs, kHorikawa::ParticleUtility rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDir.Dot(lhs.GetPosition() - m_cameraPosition) > cameraDir.Dot(rhs.GetPosition() - m_cameraPosition);
		});

	//	表示に使う頂点リストに登録されているデータを全削除
	m_vertices.clear();
	//	パーティクル情報から、表示に使う頂点リストを生成する
	for (kHorikawa::ParticleUtility& li : m_particleUtility)
	{
		if (cameraDir.Dot(li.GetPosition() - m_cameraPosition) < 0.0f) {
			//	内積の結果がマイナスの場合はカメラの後ろなので表示する必要なし
			continue;
		}

		DirectX::VertexPositionColorTexture vPCT;

		vPCT.position = DirectX::XMFLOAT3(li.GetPosition());
		//	テクスチャの色
		vPCT.color = DirectX::XMFLOAT4(li.GetNowColor());

		vPCT.textureCoordinate = DirectX::XMFLOAT2(li.GetNowScale().x, 0.0f);

		m_vertices.push_back(vPCT);
	}

	//	表示する点がない場合は描画を終わる
	if (m_vertices.empty())
	{
		return;
	}

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	ConstBuffer cbuff;
	//	ビュー設定
	cbuff.matView = view.Transpose();
	//	プロジェクション設定
	cbuff.matProj = proj.Transpose();
	//	ワールド設定
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	cbuff.Time = DirectX::SimpleMath::Vector4(m_timer, 0, 0, 0);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	//	頂点シェーダもピクセルシェーダも、同じ値を渡す
	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	////	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_states->DepthRead(), 0);

	//	カリングはなし
	context->RSSetState(m_states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);

	//	Create関数で読み込んだ画像をピクセルシェーダに登録する
	for (int i = 0; i < m_texture.size(); i++)
	{
		//	for文で一気に設定する
		context->PSSetShaderResources(i, 1, m_texture[i].GetAddressOf());
	}

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画
	m_batch->Begin();
	//	ジオメトリシェーダでPointを受け取ることになっているため、
	//	ここではD3D11_PRIMITIVE_TOPOLOGY_POINTLISTを使う
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertices[0], m_vertices.size());
	m_batch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
}

/// <summary>
/// ビルボード作成関数
/// </summary>
/// <param name="target">カメラターゲット（注視点）</param>
/// <param name="eye">カメラアイ（カメラ座標）</param>
/// <param name="up">上向きベクトル（基本はYのみ１のベクトル）</param>
void WIS::BombEffect::CreateBillboard(DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 eye, DirectX::SimpleMath::Vector3 up)
{
	
	m_billboard =
		DirectX::SimpleMath::Matrix::CreateBillboard(DirectX::SimpleMath::Vector3::Zero, eye - target, up);

	//	回転情報を設定する
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
	//	180度逆を向いているので、補正するための情報を入れる
	rot._11 = -1;
	rot._33 = -1;

	m_cameraPosition = eye;
	m_cameraTarget = target;
	m_billboard = rot * m_billboard;
}

void WIS::BombEffect::hit(DirectX::SimpleMath::Vector3 pos)
{
	// 乱数生成器の準備
	std::random_device seed;
	std::default_random_engine engine(seed());

	// 各種分布の設定
	std::uniform_real_distribution<float> dist(0.0f, DirectX::XM_2PI);   // 角度用
	std::uniform_real_distribution<float> spread(-1.0f, 1.0f);          // 汎用揺らぎ用
	std::uniform_real_distribution<float> sparkLifeDist(0.05f, 0.25f);  // 火花の寿命(かなり短め)
	std::uniform_real_distribution<float> smokeLifeDist(1.0f, 2.0f);    // 煙の寿命

	// --- 1. 爆炎コア (中央で一瞬だけ眩しく光る) ---
	for (int i = 0; i < 2; i++)
	{
		m_particleUtility.emplace_back(
			0.3f,                                    // 生存時間
			pos,                                     // 座標
			DirectX::SimpleMath::Vector3::Zero,      // 速度0
			DirectX::SimpleMath::Vector3::Zero,      // 加速度0
			DirectX::SimpleMath::Vector3(1, 1, 1),   // 開始サイズ
			DirectX::SimpleMath::Vector3(12, 12, 1), // 終了サイズ(一気に広がる)
			DirectX::SimpleMath::Color(2.0f, 2.0f, 1.5f, 1.0f), // 開始(白に近い黄色)
			DirectX::SimpleMath::Color(1.0f, 0.2f, 0.0f, 0.0f)  // 終了(赤から透明へ)
		);
	}

	// --- 2. 飛散する火花 (ランダムな寿命で飛び散る) ---
	for (int i = 0; i < 30; i++)
	{
		// 全方位へランダムに飛ばす
		DirectX::SimpleMath::Vector3 velocity(spread(engine), spread(engine), spread(engine));
		velocity.Normalize();
		velocity *= (15.0f + spread(engine) * 5.0f); // 速度のバラつき

		m_particleUtility.emplace_back(
			sparkLifeDist(engine),               // 短いランダム寿命
			pos,
			velocity,
			DirectX::SimpleMath::Vector3(0, -9.8f, 0), // 重力の影響
			DirectX::SimpleMath::Vector3(0.4f, 0.4f, 1),
			DirectX::SimpleMath::Vector3::Zero,
			DirectX::SimpleMath::Color(1.0f, 0.9f, 0.4f, 1.0f),
			DirectX::SimpleMath::Color(1.0f, 0.1f, 0.0f, 0.0f)
		);
	}

	// --- 3. 煙 (円形配置にランダムな揺らぎを追加) ---
	int smokeCount = 18;
	for (int i = 0; i < smokeCount; i++)
	{
		// 基本の円形角度に少しノイズを混ぜる
		float angle = (DirectX::XM_2PI / smokeCount) * i + (spread(engine) * 0.2f);

		// 広がる速さにもバラつきを持たせる
		float speed = 4.0f + spread(engine) * 1.5f;

		DirectX::SimpleMath::Vector3 velocity(
			cosf(angle) * speed,
			0.8f + spread(engine) * 0.4f, // わずかに上昇気流
			sinf(angle) * speed
		);

		m_particleUtility.emplace_back(
			smokeLifeDist(engine),                // 煙ごとのランダム寿命
			pos,
			velocity,
			DirectX::SimpleMath::Vector3(0, 0.4f, 0), // ゆっくり上昇
			DirectX::SimpleMath::Vector3(2, 2, 1),    // 初期サイズ
			DirectX::SimpleMath::Vector3(8, 8, 1),    // 最終サイズ(膨らむ)
			DirectX::SimpleMath::Color(0.2f, 0.2f, 0.2f, 0.6f), // 濃いグレー
			DirectX::SimpleMath::Color(0.05f, 0.05f, 0.05f, 0.0f) // 消える
		);
	}
}