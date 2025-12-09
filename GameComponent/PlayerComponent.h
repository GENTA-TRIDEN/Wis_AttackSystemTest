#pragma once
#include "WISLib/GameObject/Component/ComponentBase.h"
#include "WISLib/WISLibTags.h"


namespace WIS {
    class PlayerComponent : public WIS::ComponentBase
    {
    public:
		PlayerComponent();
		~PlayerComponent();

        void OnStart(UserResources* userResources) override;
        void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj) override;
        void OnRelease() override;

    };
}

