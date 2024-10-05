#include "pch.h"
#include "StaticTextures.h"
#include <iostream>

StaticTextures::StaticTextures()
	:m_pTextureMap{}
{
	LoadTexture("Controls", "Screens/Controls.png");
	LoadTexture("DeathScreen", "Screens/DeathScreen.png");
	LoadTexture("EndScreen", "Screens/EndScreen.png");

	LoadTexture("HUDTopLeft", "HUD/TopLeftHUD.png");
	LoadTexture("HUDTopRightShroud", "HUD/HealthShieldShroud.png");
	LoadTexture("HUDHealthFill", "HUD/HealthFill.png");
	LoadTexture("HUDShieldFill", "HUD/ShieldFill.png");
	LoadTexture("HUDAmmoType", "HUD/AmmoType.png");
	LoadTexture("HUDAmmoBullets", "HUD/BulletsHUD.png");
	LoadTexture("HUDNumbers", "HUD/NumbersHUD.png");

	LoadTexture("MasterChiefTorso", "MasterChief/MasterChief_Top_GunsAction.png");
	LoadTexture("MasterChiefLegs", "MasterChief/MasterChief_Legs.png");
	LoadTexture("MasterChiefDeath", "MasterChief/MasterChief_Death.png");

	LoadTexture("ImpPistol", "Covenant/ImpPistol.png");
	LoadTexture("ImpNeedle", "Covenant/ImpNeedle.png");
	LoadTexture("EliteRifle", "Covenant/EliteRifle.png");
	LoadTexture("Jackal", "Covenant/Jackal.png");
	LoadTexture("Hunter", "Covenant/Hunter.png");

	LoadTexture("Casing", "Weapons/Weapon_Bullets.png");
	LoadTexture("GunSkeleton", "Weapons/Weapon_Skeleton.png");
	//LoadTexture("Grenade", "Weapons/Weapon_Grenade.png");
	LoadTexture("Projectile", "Weapons/Weapon_Projectiles.png");
}

StaticTextures::~StaticTextures()
{
	for (const auto& texturePair : m_pTextureMap)
	{
		delete texturePair.second;
	}
}

void StaticTextures::LoadTexture(const std::string& key, const std::string& path)
{
	m_pTextureMap.insert({ key, new Texture{path} });
}

const Texture* StaticTextures::GetTexture(const std::string& key) const
{
	auto textureIt = m_pTextureMap.find(key);

	if (textureIt != m_pTextureMap.end())
	{
		return textureIt->second;
	}
	else
	{
		std::cout << "Provided string doesn't match any texture\nNo texture was loaded\n\n";
		return nullptr;
	}
}