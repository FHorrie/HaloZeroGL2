#pragma once
#include "StaticTextures.h"
#include "MasterChiefBase.h"

class HUD final
{
public:
	explicit HUD(const StaticTextures& textures, const Rectf& viewPort, float maxShield, float maxHealth);
	HUD(const HUD& other) = delete;
	HUD& operator= (const HUD& other) = delete;
	HUD(const HUD&& other) = delete;
	HUD& operator= (const HUD&& other) = delete;
	~HUD() = default;

	void Update(float currentShield, float currentHealth, GunType currentWeapon, int ammo, int reserve);
	void Draw() const;

private:
	const Rectf m_ViewPort;

	const Texture* m_pHUDTopL;
	const Texture* m_pHUDTopRShroud;
	const Texture* m_pHUDHealthFill;
	const Texture* m_pHUDShieldFill;
	const Texture* m_pHUDAmmoType;
	const Texture* m_pHUDAmmoBullets;
	const Texture* m_pHUDNumbers;

	const float m_Offset;

	float m_CurrentShield;
	const float m_MaxShield;
	float m_CurrentHealth;
	const float m_MaxHealth;

	GunType m_GunType;

	int m_AmmoCount;
	int m_AmmoReserve;

	bool m_EnergyWeapon;

	void DrawBaseHUD() const;
	void DrawAmmoHUD() const;
	void DrawAmmoNumbers() const;

	void SetCurrentShield(float currentShield);
	void SetCurrentHealth(float currentHealth);
	void SetType(GunType type);
	void SetAmmo(int ammo, int reserve);

};
