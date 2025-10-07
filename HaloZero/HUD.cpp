#include "pch.h"
#include "HUD.h"
#include "Texture.h"

HUD::HUD(const StaticTextures& textures, const Rectf& viewPort, float maxShield, float maxHealth)
	: m_ViewPort{viewPort}
	, m_pHUDTopL{textures.GetTexture("HUDTopLeft")}
	, m_pHUDTopRShroud{textures.GetTexture("HUDTopRightShroud")}
	, m_pHUDHealthFill{textures.GetTexture("HUDHealthFill")}
	, m_pHUDShieldFill{textures.GetTexture("HUDShieldFill")}
	, m_pHUDAmmoType{textures.GetTexture("HUDAmmoType")}
	, m_pHUDAmmoBullets{textures.GetTexture("HUDAmmoBullets")}
	, m_pHUDNumbers{textures.GetTexture("HUDNumbers")}
	, m_Offset{10.f}
	, m_CurrentShield{maxShield}
	, m_MaxShield{maxShield}
	, m_CurrentHealth{maxHealth}
	, m_MaxHealth{maxHealth}
	, m_GunType{}
	, m_AmmoCount{}
	, m_AmmoReserve{}
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

	Rectf shieldSrc{ 0, 0, m_pHUDShieldFill->GetWidth() * shieldPercentage, m_pHUDShieldFill->GetHeight() };
	Rectf healthSrc{ 0, 0, m_pHUDHealthFill->GetWidth() * healthPercentage, m_pHUDHealthFill->GetHeight() };

	Rectf AmmoTypeSrc{ m_pHUDAmmoType->GetWidth() / 2 * int(m_EnergyWeapon), 0, m_pHUDAmmoType->GetWidth() / 2, m_pHUDAmmoType->GetHeight() };

	m_pHUDHealthFill->Draw(Rectf(m_ViewPort.width - m_pHUDHealthFill->GetWidth() - m_Offset - 45, m_ViewPort.height - m_Offset - m_pHUDHealthFill->GetHeight() - 25, m_pHUDHealthFill->GetWidth() - (m_pHUDHealthFill->GetWidth() - healthSrc.width), m_pHUDHealthFill->GetHeight()), healthSrc);
	m_pHUDShieldFill->Draw(Rectf(m_ViewPort.width - m_pHUDShieldFill->GetWidth() - m_Offset - 2, m_ViewPort.height - m_Offset - m_pHUDShieldFill->GetHeight() - 2, m_pHUDShieldFill->GetWidth() - (m_pHUDShieldFill->GetWidth() - shieldSrc.width), m_pHUDShieldFill->GetHeight()), shieldSrc);
	m_pHUDTopL->Draw(Rectf(m_Offset, m_ViewPort.height - m_Offset - m_pHUDTopL->GetHeight(), m_pHUDTopL->GetWidth(), m_pHUDTopL->GetHeight()));
	m_pHUDTopRShroud->Draw(Rectf(m_ViewPort.width - m_pHUDTopRShroud->GetWidth() - m_Offset, m_ViewPort.height - m_Offset - m_pHUDTopRShroud->GetHeight(), m_pHUDTopRShroud->GetWidth(), m_pHUDTopRShroud->GetHeight()));
	m_pHUDAmmoType->Draw(Rectf(m_Offset + 60, m_ViewPort.height - m_Offset - m_pHUDAmmoType->GetHeight() - 5, m_pHUDAmmoType->GetWidth() / 2, m_pHUDAmmoType->GetHeight()), AmmoTypeSrc);
}

void HUD::DrawAmmoHUD() const
{
	bool drawAmmo{};
	int magSize{};
	int rowSplitCount{};
	int row{};
	
	Rectf srcRect{};
	srcRect.width = m_pHUDAmmoBullets->GetWidth() / 2;
	srcRect.height = m_pHUDAmmoBullets->GetHeight() / 3;

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

	srcRect.bottom = row * srcRect.height;

	if (drawAmmo)
	{
		for (int i{}; i < magSize; i++)
		{
			bool bulletUsed{ i >= m_AmmoCount };
			int heightDiff{};
			if (rowSplitCount)
			{
				heightDiff = i / (magSize / rowSplitCount);
			}
			else heightDiff = 0;

			srcRect.left = int(bulletUsed) * srcRect.width;

			if (rowSplitCount)
			{
				Rectf dstRect{ m_Offset * (i % (magSize / rowSplitCount) + 1), m_ViewPort.height - m_pHUDTopL->GetHeight() - m_pHUDAmmoBullets->GetHeight() / 3 * (0.6f + 0.6f*(heightDiff)) - m_Offset - 4, m_pHUDAmmoBullets->GetWidth() / 2, m_pHUDAmmoBullets->GetHeight() / 3 };
				m_pHUDAmmoBullets->Draw(dstRect, srcRect);
			}
			else
			{
				Rectf dstRect{ m_Offset * (i + 1), m_ViewPort.height - m_pHUDTopL->GetHeight() - m_pHUDAmmoBullets->GetHeight() / 3 * (1 + (heightDiff)) - m_Offset - 4, m_pHUDAmmoBullets->GetWidth() / 2, m_pHUDAmmoBullets->GetHeight() / 3 };
				m_pHUDAmmoBullets->Draw(dstRect, srcRect);
			}
		}
	}
}

void HUD::DrawAmmoNumbers() const
{
	int amtOfNrs{ 10 };
	int nrOffset{ 6 };

	Rectf srcRect{};
	srcRect.width = m_pHUDNumbers->GetWidth() / amtOfNrs;
	srcRect.height = m_pHUDNumbers->GetHeight();

	Rectf destRect{};
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;
	destRect.bottom = m_ViewPort.height - m_Offset - m_pHUDNumbers->GetHeight() - 3;

	if (!m_EnergyWeapon)
	{
		if (m_AmmoReserve >= 100)
		{
			destRect.left = m_Offset + nrOffset;
			srcRect.left = (m_AmmoReserve / 100) * srcRect.width;
			m_pHUDNumbers->Draw(destRect, srcRect);
		}
		if (m_AmmoReserve >= 10)
		{
			destRect.left = m_Offset + srcRect.width + nrOffset - 1;
			srcRect.left = ((m_AmmoReserve % 100) / 10) * srcRect.width;
			m_pHUDNumbers->Draw(destRect, srcRect);
		}
		destRect.left = m_Offset + srcRect.width * 2 + nrOffset - 2;
		srcRect.left = (m_AmmoReserve % 10) * srcRect.width;
		m_pHUDNumbers->Draw(destRect, srcRect);
	}
	else
	{
		if (m_AmmoCount >= 100)
		{
			destRect.left = m_Offset + nrOffset;
			srcRect.left = (m_AmmoCount / 100) * srcRect.width;
			m_pHUDNumbers->Draw(destRect, srcRect);
		}
		if (m_AmmoCount >= 10)
		{
			destRect.left = m_Offset + srcRect.width + nrOffset - 1;
			srcRect.left = ((m_AmmoCount % 100) / 10) * srcRect.width;
			m_pHUDNumbers->Draw(destRect, srcRect);
		}
		destRect.left = m_Offset + srcRect.width * 2 + nrOffset - 2;
		srcRect.left = (m_AmmoCount % 10) * srcRect.width;
		m_pHUDNumbers->Draw(destRect, srcRect);
	}

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
		m_EnergyWeapon = false;
		break;
	case GunType::MagnumPistol:
		m_EnergyWeapon = false;
		break;
	case GunType::PlasmaPistol:
		m_EnergyWeapon = true;
		break;
	case GunType::PlasmaRifle:
		m_EnergyWeapon = true;
		break;
	case GunType::Needler:
		m_EnergyWeapon = false;
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