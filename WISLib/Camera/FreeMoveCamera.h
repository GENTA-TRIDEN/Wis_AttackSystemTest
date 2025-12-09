#pragma once
//////////////////////////////////////
// File       : [WisteryGame]
// Author     : [Fujii Genta]
// Date       : [2025-06-18]
// Summary    : [WASDで自由に動ける・マウスで視点移動]
/////////////////////////////////////
#include "CameraBase.h"
class FreeMoveCamera :public WIS::CameraBase
{
//関数群


private:
	float m_moveSpeed = 0.01f; // 移動速度

	float m_yaw;   // 水平方向（左右）の回転角度
	float m_pitch; // 垂直方向（上下）の回転角度

public:
	FreeMoveCamera(DirectX::SimpleMath::Vector3 position);
	~FreeMoveCamera();
	//派生オブジェクト特有の動き
	void UniqueInitialize()override;
	void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)override;
	void UniqueRander()override;
	void Uniquefinalize()override;




};

