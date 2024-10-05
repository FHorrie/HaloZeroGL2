#include "pch.h"
#include "GunPickup.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

GunPickup::GunPickup(const StaticTextures& textures, const Point2f& startLocation, GunType type)
	:StaticSprite("GunSkeleton", textures, startLocation)
	, m_Rows{ 5 }
	, m_HitBox{startLocation.x - m_pSpriteTexture->GetWidth() / 2, startLocation.y - m_pSpriteTexture->GetHeight() / (2 * m_Rows) , 
				m_pSpriteTexture->GetWidth(), m_pSpriteTexture->GetHeight() / m_Rows}
	, m_GunType{type}
	, m_Ammo{}
	, m_Reserve{}
	, m_DirVelocity{ 0, 0 }
	, m_Acceleration{ 0, -800 }
{
	switch (type)
	{
	case GunType::smart:
		m_Ammo = 15 + rand() % 34;
		m_Reserve = 30 + rand() % 90;
		break;
	case GunType::magnum:
		m_Ammo = 4 + rand() % 6;
		m_Reserve = 30 + rand() % 90;
		break;
	case GunType::pistol:
		m_Ammo = 70 + rand() % 25;
		m_Reserve = 0;
		break;
	case GunType::rifle:
		m_Ammo = 70 + rand() % 25;
		m_Reserve = 0;
		break;
	case GunType::needle:
		m_Ammo = 12 + rand() % 6;
		m_Reserve = 10 + rand() % 30;
		break;
	default:
		break;
	}

}

GunPickup::GunPickup(const StaticTextures& textures, const Point2f& startLocation, GunType type, int ammo, int reserve)
	:StaticSprite("GunSkeleton", textures, startLocation)
	, m_Rows{ 5 }
	, m_HitBox{ startLocation.x - m_pSpriteTexture->GetWidth() / 2, startLocation.y - m_pSpriteTexture->GetHeight() / (2 * m_Rows) ,
				m_pSpriteTexture->GetWidth(), m_pSpriteTexture->GetHeight() / m_Rows }
	, m_GunType{ type }
	, m_Ammo{ammo}
	, m_Reserve{reserve}
	, m_DirVelocity{ 0, 0 }
	, m_Acceleration{ 0, -800 }
{
}

void GunPickup::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth();
	srcRect.height = m_pSpriteTexture->GetHeight() / m_Rows;
	srcRect.left = 0;
	srcRect.bottom = srcRect.height * (1 + int(m_GunType));

	Rectf destRect{};
	destRect.left = m_HitBox.left;
	destRect.bottom = m_HitBox.bottom;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	m_pSpriteTexture->Draw(destRect, srcRect);
}

void GunPickup::Update(float elapsedSec, const Level& level)
{
	UpdatePosition(elapsedSec, level);

}

void GunPickup::UpdatePosition(float elapsedSec, const Level& level)
{
	if (!level.IsOnGround(m_HitBox, m_DirVelocity))
	{
		//std::cout << "notOnGround ";
		m_DirVelocity.y += m_Acceleration.y * elapsedSec;
	}
	else;

	m_HitBox.bottom += m_DirVelocity.y * elapsedSec;

	level.HandleCollision(m_HitBox, m_DirVelocity);
}

bool GunPickup::CheckOverlap(const Rectf& actorShape) const
{
	if (utils::IsOverlapping(m_HitBox, actorShape))
		return true;
	else return false;
}

GunType GunPickup::GetGunType() const
{
	return m_GunType;
}

int GunPickup::GetAmmo() const
{
	return m_Ammo;
}

int GunPickup::GetReserve() const
{
	return m_Reserve;
}