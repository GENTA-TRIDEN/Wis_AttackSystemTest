#include "pch.h"
#include "PartStatsComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "CharacterComponent.h" 
#include <iostream> // デバッグ/ログ出力用

using namespace WIS;

// 衝突時にパーツ固有の特殊効果を適用する (WeaponComponentからの委譲を受けて実行)
void PartStatsComponent::ApplyOnHitEffect(std::shared_ptr<WIS::GameObject> hitTarget, float baseDamage)
{

}