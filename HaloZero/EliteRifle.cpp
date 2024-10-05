#include "pch.h"
#include "EliteRifle.h"
#include "Texture.h"
//#include <iostream>

const float EliteRifle::MELEETIME{ 1.5f };

EliteRifle::EliteRifle(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation)
	:EnemyBase("EliteRifle", textures, sounds, startLocation, 8, 5, 6, 80, 150.f, EnemyGunType::rifle)
	, m_pEliteDeath{sounds.GetSoundEffect("EliteDeath")}
	, m_MeleeAccuTime{}
{
}

void EliteRifle::Update(float elapsedSec, const Level& level, const Rectf& actorShape)
{
	StateSwitch(actorShape, elapsedSec);
	HandleShootHitBox(180);
	HandleDetectionBox(700);
	UpdatePosition(elapsedSec, level, 100.f);
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);
}

void EliteRifle::UpdateCurrentFrame(float elapsedSec)
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

			else if (m_State == EnemyState::melee)
			{
				m_Meleeing = true;
				m_MeleeAccuTime -= MELEETIME;
				ChangeState(EnemyState::shooting);
			}
			else;
		}
	}
}

void EliteRifle::UpdateFramesState()
{
	switch (m_State)
	{
	case EnemyState::holding:
		m_nFrames = 1;
		m_CurrentRow = 0;
		m_Looped = false;
		break;
	case EnemyState::shooting:
		m_nFrames = 2;
		m_CurrentRow = 0;
		m_Looped = true;
		break;
	case EnemyState::running:
		m_nFrames = 6;
		m_CurrentRow = 1;
		m_Looped = true;
		break;
	case EnemyState::melee:
		m_nFrames = 2;
		m_CurrentRow = 2;
		m_Looped = false;
		break;
	case EnemyState::grenade:
		m_nFrames = 2;
		m_CurrentRow = 3;
		m_Looped = false;
		break;
	case EnemyState::dead:
		m_nFrames = 3;
		m_CurrentRow = 4;
		m_Looped = false;
		break;
	}
}

void EliteRifle::StateSwitch(const Rectf& actorShape, float elapsedSec)
{
	if (m_State != EnemyState::dead)
	{
		if (utils::IsOverlapping(actorShape, m_HitBox))
		{
			m_MeleeAccuTime += elapsedSec;
			float reqSec{ 1.6f };
			if (m_State != EnemyState::melee && m_MeleeAccuTime >= reqSec)
			{
				ChangeState(EnemyState::melee);
			}
			else;
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else if (utils::IsOverlapping(actorShape, m_ShootHitBox))
		{
			if (m_State != EnemyState::shooting)
			{
				ChangeState(EnemyState::shooting);
			}
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else if (utils::IsOverlapping(actorShape, m_DetectionBox))
		{
			if (m_State != EnemyState::running)
			{
				ChangeState(EnemyState::running);
			}
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else
			if (m_State != EnemyState::holding)
			{
				ChangeState(EnemyState::holding);
			}
	}
	else 
	{
		if (!m_Dead)
		{
			m_pEliteDeath->Play(0);
			m_Dead = true;
		}
	}
}