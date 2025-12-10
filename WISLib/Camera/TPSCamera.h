// TPSCamera.h
#pragma once
#include "CameraBase.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <Mouse.h>

namespace WIS {
    // ... (必要な前方宣言などがあればここに追加)

    class TPSCamera : public CameraBase
    {
    private:
        float m_yaw;
        float m_pitch;

        // 追加: プレイヤーの位置を保持する変数
        DirectX::SimpleMath::Vector3 m_PlayerPos;

    public:
        TPSCamera(DirectX::SimpleMath::Vector3 position);
        ~TPSCamera();

        // CameraBaseの仮想関数をオーバーライド
        void UniqueInitialize() override;
        void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state) override;
        void UniqueRander() override;
        void Uniquefinalize() override;

        float GetYaw() const { return m_yaw; }

        // 追加: 外部からプレイヤーの位置を設定するためのsetter
        void SetPlayerPosition(const DirectX::SimpleMath::Vector3& pos) { m_PlayerPos = pos; }
    };

}