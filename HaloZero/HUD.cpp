#include "pch.h"
#include "HUD.h"
#include "Texture.h"
#include <iostream>

HUD::HUD(const StaticTextures& textures, const Rectf& viewPort, float maxShield, float maxHealth)
	: m_ViewPort{viewPort}
	, m_TopLeftSpritePtr{textures.GetTexture("HUDTopLeft")}
	, m_ShroudSpritePtr{textures.GetTexture("HUDTopRightShroud")}
	, m_HealthFillSpritePtr{textures.GetTexture("HUDHealthFill")}
	, m_ShieldFillSpritePtr{textures.GetTexture("HUDShieldFill")}
	, m_AmmoTypeSpritePtr{textures.GetTexture("HUDAmmoType")}
	, m_AmmoBulletsSpritePtr{textures.GetTexture("HUDAmmoBullets")}
	, m_NumbersSpritePtr{textures.GetTexture("HUDNumbers")}
	, m_CurrentShield{maxShield}
	, m_MaxShield{maxShield}
	, m_CurrentHealth{maxHealth}
	, m_MaxHealth{maxHealth}
{
}

void HUD::Update(float currentShield, float currentHealth, GunType currentWeapon, int ammo, int reserve)
{
	SetCurrentShield(currentShield);
	SetCurrentHealth(currentHealth);
	SetType(currentWeapon);
	SetAmmo(ammo, reserve);
}

void HUD::Draw() const
{
	DrawBaseHUD();
	DrawAmmoHUD();
	DrawAmmoNumbers();
}

void HUD::DrawBaseHUD() const
{
	float shieldPercentage{ m_CurrentShield / m_MaxShield };
	float healthPercentage{ m_CurrentHealth / m_MaxHealth };

	Rectf shieldSrc{ 0, 0, m_ShieldFillSpritePtr->GetWidth() * shieldPercentage, m_ShieldFillSpritePtr->GetHeight() };
	Rectf healthSrc{ 0, 0, m_HealthFillSpritePtr->GetWidth() * healthPercentage, m_HealthFillSpritePtr->GetHeight() };

	Rectf AmmoTypeSrc{ m_AmmoTypeSpritePtr->GetWidth() / 2 * int(m_IsEnergyWeapon), 0, m_AmmoTypeSpritePtr->GetWidth() / 2, m_AmmoTypeSpritePtr->GetHeight() };

	m_HealthFillSpritePtr->Draw(Rectf(m_ViewPort.width - m_HealthFillSpritePtr->GetWidth() - m_Offset - 45, m_ViewPort.height - m_Offset - m_HealthFillSpritePtr->GetHeight() - 25, m_HealthFillSpritePtr->GetWidth() - (m_HealthFillSpritePtr->GetWidth() - healthSrc.width), m_HealthFillSpritePtr->GetHeight()), healthSrc);
	m_ShieldFillSpritePtr->Draw(Rectf(m_ViewPort.width - m_ShieldFillSpritePtr->GetWidth() - m_Offset - 2, m_ViewPort.height - m_Offset - m_ShieldFillSpritePtr->GetHeight() - 2, m_ShieldFillSpritePtr->GetWidth() - (m_ShieldFillSpritePtr->GetWidth() - shieldSrc.width), m_ShieldFillSpritePtr->GetHeight()), shieldSrc);
	m_TopLeftSpritePtr->Draw(Rectf(m_Offset, m_ViewPort.height - m_Offset - m_TopLeftSpritePtr->GetHeight(), m_TopLeftSpritePtr->GetWidth(), m_TopLeftSpritePtr->GetHeight()));
	m_ShroudSpritePtr->Draw(Rectf(m_ViewPort.width - m_ShroudSpritePtr->GetWidth() - m_Offset, m_ViewPort.height - m_Offset - m_ShroudSpritePtr->GetHeight(), m_ShroudSpritePtr->GetWidth(), m_ShroudSpritePtr->GetHeight()));
	m_AmmoTypeSpritePtr->Draw(Rectf(m_Offset + 60, m_ViewPort.height - m_Offset - m_AmmoTypeSpritePtr->GetHeight() - 5, m_AmmoTypeSpritePtr->GetWidth() / 2, m_AmmoTypeSpritePtr->GetHeight()), AmmoTypeSrc);
}

void HUD::DrawAmmoHUD() const
{
	bool drawAmmo{};
	int magSize{};
	int rowSplitCount{};
	int row{};
	
	switch (m_GunType)
	{
	case GunType::SmartRifle:
		drawAmmo = true;
		magSize = 60;
		rowSplitCount = 4;
		row = 3;
		break;
	case GunType::MagnumPistol:
		drawAmmo = true;
		magSize = 10;
		row = 2;
		break;
	case GunType::PlasmaPistol:
		drawAmmo = false;
		break;
	case GunType::PlasmaRifle:
		drawAmmo = false;
		break;
	case GunType::Needler:
		drawAmmo = true;
		magSize = 20;
		row = 1;
		break;
	}

	if (drawAmmo)
	{
		Rectf srcRect{0, 0, m_AmmoBulletsSpritePtr->GetWidth() / 2, m_AmmoBulletsSpritePtr->GetHeight() / 3};
		srcRect.bottom = row * srcRect.height;
		
		for (int i{}; i < magSize; i++)
		{
			const bool bulletUsed{ i >= m_AmmoCount };
			const int heightDiff{ rowSplitCount > 0 ? i / (magSize / rowSplitCount) : 0 };

			srcRect.left = static_cast<int>(bulletUsed) * srcRect.width;

			Rectf dstRect{0, 0, m_AmmoBulletsSpritePtr->GetWidth() / 2, m_AmmoBulletsSpritePtr->GetHeight() / 3 };
			if (rowSplitCount > 0)
			{
				dstRect.left = m_Offset * (i % (magSize / rowSplitCount) + 1),
				dstRect.bottom = m_ViewPort.height - m_TopLeftSpritePtr->GetHeight() - m_AmmoBulletsSpritePtr->GetHeight() / 3 * (0.6f + 0.6f * heightDiff) - m_Offset - 4;
			}
			else
			{
				dstRect.left = m_Offset * (i + 1);
				dstRect.bottom = m_ViewPort.height - m_TopLeftSpritePtr->GetHeight() - m_AmmoBulletsSpritePtr->GetHeight() / 3 * (1 + (heightDiff)) - m_Offset - 4;
			}

			m_AmmoBulletsSpritePtr->Draw(dstRect, srcRect);
		}
	}
}

void HUD::DrawAmmoNumbers() const
{
	const int amtOfNrs{ 10 };
	const int nrOffset{ 6 };

	Rectf srcRect{};
	srcRect.width = m_NumbersSpritePtr->GetWidth() / amtOfNrs;
	srcRect.height = m_NumbersSpritePtr->GetHeight();

	Rectf destRect{};
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;
	destRect.bottom = m_ViewPort.height - m_Offset - m_NumbersSpritePtr->GetHeight() - 3;

	const int ammoReference{ m_IsEnergyWeapon ? m_AmmoCount : m_AmmoReserve };

	if (ammoReference >= 100)
	{
		destRect.left = m_Offset + nrOffset;
		srcRect.left = (ammoReference / 100) * srcRect.width;
		m_NumbersSpritePtr->Draw(destRect, srcRect);
	}
	if (ammoReference >= 10)
	{
		destRect.left = m_Offset + srcRect.width + nrOffset - 1;
		srcRect.left = ((ammoReference % 100) / 10) * srcRect.width;
		m_NumbersSpritePtr->Draw(destRect, srcRect);
	}
	destRect.left = m_Offset + srcRect.width * 2 + nrOffset - 2;
	srcRect.left = (ammoReference % 10) * srcRect.width;
	m_NumbersSpritePtr->Draw(destRect, srcRect);
}

void HUD::SetCurrentShield(float currentShield)
{
	(currentShield <= 0.1f) ? m_CurrentShield = 0.1f : m_CurrentShield = currentShield;
}

void HUD::SetCurrentHealth(float currentHealth)
{
	(currentHealth <= 0.1f) ? m_CurrentHealth = 0.1f : m_CurrentHealth = currentHealth;
}

void HUD::SetType(GunType type)
{
	m_GunType = type;

	switch (m_GunType)
	{
	case GunType::SmartRifle:
		m_IsEnergyWeapon = false;
		break;
	case GunType::MagnumPistol:
		m_IsEnergyWeapon = false;
		break;
	case GunType::PlasmaPistol:
		m_IsEnergyWeapon = true;
		break;
	case GunType::PlasmaRifle:
		m_IsEnergyWeapon = true;
		break;
	case GunType::Needler:
		m_IsEnergyWeapon = false;
		break;
	default:
		break;
	}
}

void HUD::SetAmmo(int ammo, int reserve)
{
	m_AmmoCount = ammo;
	m_AmmoReserve = reserve;
}