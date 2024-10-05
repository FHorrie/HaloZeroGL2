#include "pch.h"
#include "debugpch.h"
#include "EnemyBase.h"
#include "Texture.h"

EnemyBase::EnemyBase(const std::string& spriteType, const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation, unsigned short framerate, unsigned short rows, unsigned short cols, short startHealth, float speed, EnemyGunType gunType)
	:AnimatedSprite(spriteType, textures, startLocation, 0, framerate, rows, cols)
	, m_State{EnemyState::holding}
	, m_Gun{ gunType }

	, m_pPlasmaPistolShot{ sounds.GetSoundEffect("PlasmaPistolShot") }
	, m_pPlasmaRifleShot{ sounds.GetSoundEffect("PlasmaRifleShot") }
	, m_pNeedlerShot{ sounds.GetSoundEffect("NeedlerShot") }
	, m_pHunterShot{ sounds.GetSoundEffect("HunterShot") }

	, m_HitBox{ startLocation.x, startLocation.y, m_pSpriteTexture->GetWidth() / (m_Cols * (3.f/2.f)), m_pSpriteTexture->GetHeight() / m_Rows }
	, m_DetectionBox{}
	, m_ShootHitBox{}
	, m_CurrentRow{}
	, m_DirVelocity{0, 0}
	, m_Acceleration{0, -1200}
	, m_HorSpeed{ speed }
	, m_Health{ startHealth }
{
}
void EnemyBase::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight() / m_Rows;
	srcRect.left = (m_CurrentFrame % m_Cols) * srcRect.width;
	srcRect.bottom = srcRect.height * (m_CurrentRow + 1);

	Rectf destRect{};
	destRect.left = - m_HitBox.width/3;
	destRect.bottom = 0;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	glPushMatrix();
	glTranslatef(m_HitBox.left, m_HitBox.bottom, 0);

	if (m_IsFlipped)
	{
		glScalef(-1, 1, 1);
		glTranslatef(-m_HitBox.width, 0, 0);
	}

	m_pSpriteTexture->Draw(destRect, srcRect);

	glPopMatrix();

#ifdef _DEBUG_HITBOX
		utils::SetColor(Color4f(1.f, 0.f, 0.f, 1.f));
		utils::DrawRect(m_ShootHitBox);

		utils::SetColor(Color4f(0.1f, 0.1f, 0.7f, 1.f));
		utils::DrawRect(m_DetectionBox);

		utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
		utils::DrawRect(m_HitBox);
#endif
}

Rectf EnemyBase::GetShape() const
{
	return m_HitBox;
}

Point2f EnemyBase::GetDropPoint() const
{
	return Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + m_HitBox.height / 2);
}

void EnemyBase::ChangeState(EnemyState state)
{
	m_State = state;
	m_CurrentFrame = 0;
}

void EnemyBase::UpdateCurrentFrame(float elapsedSec)
{
	if (m_nFrames <= 1)
	{
		m_CurrentFrame = 0;
		return;
	}


	m_AccuTime += elapsedSec;

	if (m_AccuTime >= (1.f / m_Framerate))
	{
		m_AccuTime -= 1.f / m_Framerate;

		if (m_CurrentFrame < (m_nFrames - 1))
		{
			m_CurrentFrame++;
		}
		else
		{
			if (m_State == EnemyState::shooting) m_ShotFired = true;
			if (m_Looped) m_CurrentFrame = 0;
			else;
		}
	}
}

void EnemyBase::HandleShootHitBox(float boxWidth)
{
	if (m_State != EnemyState::dead)
	{
		//float boxWidth{ 150 };
		if (!m_IsFlipped)
			m_ShootHitBox = Rectf(m_HitBox.left + m_HitBox.width / 2 - boxWidth, m_HitBox.bottom, boxWidth, m_HitBox.height * 1.5f);
		else
			m_ShootHitBox = Rectf(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom, boxWidth, m_HitBox.height * 1.5f);
	}
	else
		m_ShootHitBox = Rectf();
}

void EnemyBase::HandleDetectionBox(float boxWidth)
{
	if (m_State != EnemyState::dead)
	{
		//float boxWidth{ 700 };
		m_DetectionBox = Rectf(m_HitBox.left - boxWidth / 2, m_HitBox.bottom, boxWidth + m_HitBox.width, m_HitBox.height * 2);
	}
	else
		m_DetectionBox = Rectf();

}

void EnemyBase::UpdatePosition(float elapsedSec, const Level& level, float hopForce)
{
	if (!level.IsOnGround(m_HitBox, m_DirVelocity))
	{
		m_DirVelocity.y += m_Acceleration.y * elapsedSec;
	}
	else
	{
		switch (m_State)
		{
		case EnemyState::holding:
			m_DirVelocity = Vector2f{ 0, 0 };
			break;
		case EnemyState::shooting:
			m_DirVelocity = Vector2f{ 0, 0 };
			break;
		case EnemyState::running:
			m_DirVelocity.x = m_HorSpeed;
			break;
		case EnemyState::melee:
			m_DirVelocity = Vector2f{ 0, 0 };
			break;
		case EnemyState::grenade:
			m_DirVelocity = Vector2f{ 0, 0 };
			break;
		case EnemyState::dead:
			if (!m_AnimPlayed)
			{
				++m_HitBox.bottom; //make sure the enemy hops when dead
				m_DirVelocity = Vector2f{ -m_HorSpeed, hopForce };
				m_AnimPlayed = true;
				m_HitBox.height /= 3;
			}
			else m_DirVelocity = Vector2f{ 0,0 };
			
			break;
		default:
			break;
		}
	}

	(!m_IsFlipped) ? m_HitBox.left -= m_DirVelocity.x * elapsedSec : m_HitBox.left += m_DirVelocity.x * elapsedSec;

	m_HitBox.bottom += m_DirVelocity.y * elapsedSec;

	level.HandleCollision(m_HitBox, m_DirVelocity);
}

void EnemyBase::TakeDamage(int damage)
{
	m_Health -= damage;
	if (m_Health <= 0)
	{
		if (m_State != EnemyState::dead)
		{
			ChangeState(EnemyState::dead);
		}
	}
}

bool EnemyBase::IsDead() const
{
	if (m_State == EnemyState::dead)
		return true;
	else
		return false;
}

bool EnemyBase::IsShooting()
{
	bool tempShot{ m_ShotFired };

	if (m_ShotFired)
	{
		switch (m_Gun)
		{
		case EnemyGunType::pistol:
			m_pPlasmaPistolShot->Play(0);
			break;
		case EnemyGunType::rifle:
			m_pPlasmaRifleShot->Play(0);
			break;
		case EnemyGunType::needle:
			m_pNeedlerShot->Play(0);
			break;
		case EnemyGunType::none:
			m_pHunterShot->Play(0);
			break;
		}
	}

	m_ShotFired = false;
	//std::cout << tempShot;
	return tempShot;

}bool EnemyBase::IsMelee()
{
	bool tempMelee{ m_Meleeing };
	m_Meleeing = false;
	return tempMelee;
}

bool EnemyBase::DropWeapon() const
{
	if (m_State == EnemyState::dead && !m_AnimPlayed)
	{
		return true;
	}
	else return false;
}

EnemyGunType EnemyBase::GetWeapon() const 
{
	return m_Gun;
}

bool EnemyBase::GetFlipped() const
{
	return m_IsFlipped;
}