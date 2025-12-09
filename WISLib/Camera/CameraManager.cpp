#include "pch.h"
#include "CameraManager.h"
#include "CameraBase.h"
#include <algorithm> // std::find_if のために追加

// シングルトンインスタンスの初期化
std::unique_ptr<WIS::CameraManager> WIS::CameraManager::s_cameraManager = nullptr;

WIS::CameraManager::CameraManager() :
	NowCameraNum(0) // NowCameraNumはここで0に初期化
{
}

WIS::CameraManager* WIS::CameraManager::GetInstance()
{
	if (s_cameraManager == nullptr) {
		s_cameraManager.reset(new WIS::CameraManager);
	}
	return s_cameraManager.get();
}

void WIS::CameraManager::Initialize()
{
	// カメラが1つ以上ある場合のみ初期化
	if (!m_CameraVec.empty())
	{
		m_CameraVec[NowCameraNum].first->Initialize();
	}
}

void WIS::CameraManager::Update(const DirectX::Mouse::State& state)
{
	if (!m_CameraVec.empty())
	{
		m_CameraVec[NowCameraNum].first->Update(state);
	}
}

void WIS::CameraManager::Rander()
{
	if (!m_CameraVec.empty())
	{
		m_CameraVec[NowCameraNum].first->Rander();
	}
}

void WIS::CameraManager::Finalize()
{
	for (auto& camera : m_CameraVec)
	{
		camera.first->Finalize();
	}
	m_CameraVec.clear();
	s_cameraManager.reset();
}

int WIS::CameraManager::AddCamera(const DirectX::SimpleMath::Vector3& pos, const CameraMode& mode, const int Handle)
{

	if (mode == CameraMode::MODE_FIXED)
	{
		return AddCamera(pos, DirectX::SimpleMath::Vector3::Zero, mode, Handle);
	}

	int handle = GenerateUniqueHandle(Handle);
	// C++14以降で推奨される std::make_unique を使用
	switch (mode)
	{
	case CameraMode::MODE_FPSCAM:
		m_CameraVec.emplace_back(std::make_unique<FPSCamera>(pos), handle);
		break;
	case CameraMode::MODE_FREEMOVE:
		m_CameraVec.emplace_back(std::make_unique<FreeMoveCamera>(pos), handle);
		break;
	case CameraMode::MODE_VEHICLE:
		m_CameraVec.emplace_back(std::make_unique<VehicleCamera>(pos), handle);
		break;
	case CameraMode::MODE_TPSCAM:
		m_CameraVec.emplace_back(std::make_unique<TPSCamera>(pos), handle);
		break;

	default:
		break;
	}

	return handle;
}

int WIS::CameraManager::AddCamera(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const CameraMode& mode, const int Handle)
{
	int handle = GenerateUniqueHandle(Handle);

	switch (mode)
	{
	case CameraMode::MODE_FIXED:
		m_CameraVec.emplace_back(std::make_unique<FixedCamera>(pos, target), handle);
		break;

	default:
		// targetが不要なカメラの場合は、既存のAddCameraを呼び出して処理
		return AddCamera(pos, mode, handle);
	}

	return handle;
}

void WIS::CameraManager::RequestNextCamera(bool Next)
{
	if (m_CameraVec.empty()) return;

	if (Next) {
		NowCameraNum = (NowCameraNum + 1) % m_CameraVec.size();
	}
	else {
		NowCameraNum = (NowCameraNum - 1 + m_CameraVec.size()) % m_CameraVec.size();
	}
	m_CameraVec[NowCameraNum].first->Initialize();

}

int WIS::CameraManager::GenerateUniqueHandle(int Handle)
{
	auto it = std::find_if(m_CameraVec.begin(), m_CameraVec.end(),
		[Handle](const auto& camera) { // C++14のジェネリックラムダで簡潔に
			return camera.second == Handle;
		});

	if (it != m_CameraVec.end()) {
		return GenerateUniqueHandle(Handle + 1);
	}
	return Handle;
}

void WIS::CameraManager::RequestFirstCamera()
{
	if (m_CameraVec.empty()) return;
	NowCameraNum = 0;
	m_CameraVec[NowCameraNum].first->Initialize();
}

void WIS::CameraManager::RequestFirstCameraInType(const CameraMode& mode)
{
	if (m_CameraVec.empty()) return;

	auto it = std::find_if(m_CameraVec.begin(), m_CameraVec.end(),
		[mode](const auto& camera) {
			return camera.first->GetType() == mode;
		});

	if (it == m_CameraVec.end()) return;
	NowCameraNum = std::distance(m_CameraVec.begin(), it);
	m_CameraVec[NowCameraNum].first->Initialize();
}

void WIS::CameraManager::RequestCamera(int Index)
{
	if (Index < 0 || Index >= m_CameraVec.size()) {
		RequestFirstCamera();
		return;
	}
	NowCameraNum = Index;
	m_CameraVec[NowCameraNum].first->Initialize();

}

void WIS::CameraManager::RequestCameraByHandle(const int handle)
{
	auto it = std::find_if(m_CameraVec.begin(), m_CameraVec.end(),
		[handle](const auto& camera) {
			return camera.second == handle;
		});

	if (it == m_CameraVec.end()) {
		RequestFirstCamera();
		return;
	}

	NowCameraNum = std::distance(m_CameraVec.begin(), it);
	m_CameraVec[NowCameraNum].first->Initialize();
}