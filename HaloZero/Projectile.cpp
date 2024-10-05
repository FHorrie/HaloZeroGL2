#include "pch.h"
#include "debugpch.h"
#include "Projectile.h"
#include <iostream>

Projectile::Projectile(const StaticTextures& textures, const Point2f& startLocation, float angle, GunType type, bool isFlipped)
	:StaticSprite("Projectile", textures, startLocation)
	, m_BulletType{}
	, m_DrawBullet{}
	, m_Damage{}
	, m_Cols{ 4 }
	, m_Angle{ angle }
	, m_HitBox{}
	, m_BulletSpeed{}
	, m_DirVelocity{}
	, m_IsFlipped{isFlipped}	
{
	switch (type)
	{
	case GunType::smart:
		m_DrawBullet = false;
		m_BulletSpeed = 1500.f;
		m_Damage = 8;
		break;
	case GunType::magnum:
		m_DrawBullet = false;
		m_BulletSpeed = 1500.f;
		m_Damage = 12;
		break;
	case GunType::pistol:
		m_BulletType = 0;
		m_DrawBullet = true;
		m_BulletSpeed = 700.f;
		m_Damage = 8;
		break;
	case GunType::rifle:
		m_BulletType = 1;
		m_DrawBullet = true;
		m_BulletSpeed = 700.f;
		m_Damage = 10;
		break;
	case GunType::needle:
		m_BulletType = 2;
		m_DrawBullet = true;
		m_BulletSpeed = 650.f;
		m_Damage = 12;
		break;
	}

	float radOffset{ float(M_PI) / 4 };

	float newAngle{};
	float dirAngle{};

	if (!m_IsFlipped)
	{
		newAngle = angle + radOffset;
		dirAngle = angle;
	}
	else 
	{
		newAngle = angle + 3 * radOffset;
		dirAngle = angle - 4 * radOffset;
	}

	m_HitBox = Rectf(startLocation.x - m_pSpriteTexture->GetWidth() / (2 * m_Cols + 2) + 40 * cosf(newAngle), startLocation.y - m_pSpriteTexture->GetHeight() / 2 + 38 * sinf(newAngle)
		, m_pSpriteTexture->GetWidth() / (m_Cols + 2), m_pSpriteTexture->GetHeight());

	m_DirVelocity = Vector2f(m_BulletSpeed * cosf(dirAngle), m_BulletSpeed * sinf(dirAngle));
}

Projectile::Projectile(const StaticTextures& textures, const Point2f& startLocation, float angle, EnemyGunType enemyType, bool enemyFlipped)
	:StaticSprite("Projectile", textures, startLocation)
	, m_BulletType{}
	, m_DrawBullet{}
	, m_Damage{}
	, m_Cols{ 4 }
	, m_Angle{ angle }
	, m_HitBox{}
	, m_BulletSpeed{}
	, m_DirVelocity{}
	, m_IsFlipped{ enemyFlipped }
{
	switch (enemyType)
	{
	case EnemyGunType::pistol:
		m_BulletType = 0;
		m_DrawBullet = true;
		m_BulletSpeed = 700.f;
		m_Damage = 8;
		break;
	case EnemyGunType::rifle:
		m_BulletType = 1;
		m_DrawBullet = true;
		m_BulletSpeed = 700.f;
		m_Damage = 10;
		break;
	case EnemyGunType::needle:
		m_BulletType = 2;
		m_DrawBullet = true;
		m_BulletSpeed = 650.f;
		m_Damage = 15;
		break;
	case EnemyGunType::none:
		m_BulletType = 3;
		m_DrawBullet = true;
		m_BulletSpeed = 800.f;
		m_Damage = 80;
		break;
	}

	m_HitBox = Rectf(startLocation.x - m_pSpriteTexture->GetWidth() / (2 * m_Cols + 2) + 40 * cosf(angle), startLocation.y - m_pSpriteTexture->GetHeight() / 2 + 38 * sinf(angle)
		, m_pSpriteTexture->GetWidth() / (m_Cols + 2), m_pSpriteTexture->GetHeight());

	m_DirVelocity = Vector2f(m_BulletSpeed * cosf(angle), m_BulletSpeed * sinf(angle));
}

void Projectile::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight();
	srcRect.left = m_BulletType * srcRect.width;
	srcRect.bottom = 0;

	Rectf destRect{};
	destRect.left = -m_HitBox.width / 2;
	destRect.bottom = -m_HitBox.height / 2;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	glPushMatrix();
	glTranslatef(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + m_HitBox.height / 2, 0);

	glRotatef(m_Angle / float(M_PI) * 180.f, 0, 0, 1);

	if (m_IsFlipped)
		glScalef(-1, 1, 1);
	
	if (m_DrawBullet)
	{
		m_pSpriteTexture->Draw(destRect, srcRect);
	}
	
	glPopMatrix();

#ifdef _DEBUG_HITBOX
	utils::SetColor(Color4f(1.f, 1.f, 1.f, 1.f));
	utils::DrawRect(m_HitBox);
#endif
}

bool Projectile::CheckHit(const Rectf& actorShape)
{
	if (utils::IsOverlapping(m_HitBox, actorShape))
	{
		m_NeedsDeletion = true;
		return true;
	}
	else return false;
}

void Projectile::UpdatePosition(float elapsedSec, const Level& level)
{
	if (level.IsOnGround(m_HitBox, m_DirVelocity) || level.IsHittingWallLeft(m_HitBox, m_DirVelocity) || level.IsHittingWallRight(m_HitBox, m_DirVelocity))
		m_NeedsDeletion = true;

	level.HandleCollision(m_HitBox, m_DirVelocity);

	m_HitBox.left += m_DirVelocity.x * elapsedSec;
	m_HitBox.bottom += m_DirVelocity.y * elapsedSec;

	m_Position = Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + m_HitBox.height / 2);
	
}

void Projectile::CheckInScreen(const Rectf& WindowRect)
{

}

bool Projectile::CheckDeletion() const
{
	return (m_NeedsDeletion) ? true : false;
}

int Projectile::GetDamage() const
{
	return m_Damage;
}

Rectf Projectile::GetHitBox() const
{
	return m_HitBox;
}