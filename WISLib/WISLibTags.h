#pragma once
namespace WIS
{
    // ÉJÉÅÉâÇÃéÌóﬁÇíËã`Ç∑ÇÈenum
    enum class CameraMode
    {
        MODE_FPSCAM = 0,
        MODE_FREEMOVE,
        MODE_VEHICLE,
        MODE_DEBUG,
        MODE_FIXED,
        MODE_TPSCAM


    };

    enum class CollisionTag
    {
        STATIC = 0,
        DYNAMIC,
        FLOOR
    };

    enum class WeaponPartsTag 
    {
        NONE=0,
        GEAR,
        STICK,
        BOOM,
    };

	enum class SceneTag
	{
		NONE = 0,
		TITLE_SCENE,
		GAMEPLAY_SCENE,
		RESULT_SCENE,
		STACK_SCENE
	};


    enum class BoundingType
    {
        NONE = 0,
        OBB = 1 << 0, // 1
        SPHERE = 1 << 1, // 2
        AABB = 1 << 2, // 4
        FRUSTUM = 1 << 3  // 8
    };

	enum class ObjectType
	{
		NONE = 0,
		PLAYER,
		ENEMY,
		WEAPON,
		BULLET,
		ITEM,
		STAGE,
	};

	enum class AnchorPoint
	{
		TOP_LEFT = 0,
		TOP_CENTER,
		TOP_RIGHT,
		CENTER_LEFT,
		CENTER,
		CENTER_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT
	};
}