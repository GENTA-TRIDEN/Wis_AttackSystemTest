//--------------------------------------------------------------------------------------
// File: Particle.cpp
//
// パーティクルクラス
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "HitEffect.h"

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
const std::vector<D3D11_INPUT_ELEMENT_DESC> WIS::HitEffect::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/// <summary>
/// コンストラクタ
/// </summary>
WIS::HitEffect::HitEffect()
	: m_pDR(nullptr)
	, m_timer(0.0f)
{
}

/// <summary>
/// デストラクタ
/// </summary>
WIS::HitEffect::~HitEffect()
{
}

/// <summary>
/// テクスチャリソース読み込み関数
/// </summary>
/// <param name="path">相対パス(Resources/Textures/・・・.pngなど）</param>
void WIS::HitEffect::LoadTexture(const wchar_t* path)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::CreateWICTextureFromFile(m_pDR->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());

	m_texture.push_back(texture);
}

/// <summary>
/// 生成関数
/// </summary>
/// <param name="pDR">ユーザーリソース等から持ってくる</param>
void WIS::HitEffect::Create(DX::DeviceResources* pDR)
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
void WIS::HitEffect::Update(const float elapsedTime)
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
void WIS::HitEffect::CreateShader()
{
	ID3D11Device1* device = m_pDR->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	std::unique_ptr<kHorikawa::BinaryFile> VSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	std::unique_ptr<kHorikawa::BinaryFile> PSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");
	std::unique_ptr<kHorikawa::BinaryFile> GSData = kHorikawa::BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");

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
void WIS::HitEffect::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
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
void WIS::HitEffect::CreateBillboard(DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 eye, DirectX::SimpleMath::Vector3 up)
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

void WIS::HitEffect::hit(DirectX::SimpleMath::Vector3 pos)
{
	for (int i = 0; i < 20; i++)
	{
		//	完全なランダムをハードウェア的に生成するためのクラスの変数
		std::random_device seed;
		//	上記の完全なランダムは動作が遅いため、seed値の決定のみに使用する
		std::default_random_engine engine(seed());
		std::default_random_engine Iengine(seed());
		//	生成して欲しいランダムの範囲をDistributionに任せる。今回は0～2PI
		std::uniform_real_distribution<> dist(0, DirectX::XM_2PI);
		float range = 2.0f;
		//	0～2PIの値をdefault_random_engine(恐らくしばらくはmt19937)を使って取り出す
		float rand = static_cast<float>(dist(engine));
		float Irand = static_cast<float>(dist(Iengine));
		//	新しいパーティクル情報を作成する
		kHorikawa::ParticleUtility pU(
			0.2f,																							//	生存時間(s)
			pos + DirectX::SimpleMath::Vector3(range / 4.0f * cosf(rand), 0.0f, range / 4.0f * sinf(rand)),						//	基準座標
			DirectX::SimpleMath::Vector3(cosf(Irand) * 20, sinf(rand) * 20, sinf(Irand) * 20),													//	速度
			DirectX::SimpleMath::Vector3::One * 5,																//	加速度
			DirectX::SimpleMath::Vector3::One, DirectX::SimpleMath::Vector3::Zero,							//	初期スケール、最終スケール
			DirectX::SimpleMath::Color(1.f, 0.f, 0.01f, 1.f), DirectX::SimpleMath::Color(0.02f, 0.f, 0.f, 0.f)	//	初期カラー、最終カラー
		);


		//	リストに追加
		m_particleUtility.push_back(pU);

	}
}