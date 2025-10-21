#include "pch.h"
#include "Jackal.h"
#include "Texture.h"

Jackal::Jackal(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation)
	:EnemyBase("Jackal", textures, sounds, startLocation, 8, 3, 6, 65, 80.f, EnemyGunType::PlasmaPistol)
	, m_pJackalDeath{sounds.GetSoundEffect("JackalDeath")}
{
}

void Jackal::Update(float elapsedSec, const Level& level, const Rectf& actorShape)
{
	StateSwitch(actorShape, elapsedSec);
	HandleShootHitBox(180);
	HandleDetectionBox(700);
	UpdatePosition(elapsedSec, level, 150.f);
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);
}

void Jackal::UpdateFramesState()
{
	switch (m_State)
	{
	case EnemyState::Holding:
		m_nFrames = 1;
		m_CurrentRow = 0;
		m_Looped = false;
		break;
	case EnemyState::Shooting:
		m_nFrames = 2;
		m_CurrentRow = 0;
		m_Looped = true;
		break;
	case EnemyState::Running:
		m_nFrames = 6;
		m_CurrentRow = 1;
		m_Looped = true;
		break;
	case EnemyState::Dead:
		m_nFrames = 3;
		m_CurrentRow = 2;
		m_Looped = false;
		break;
	}
}

void Jackal::StateSwitch(const Rectf& actorShape, float elapsedSec)
{
	if (m_State != EnemyState::Dead)
	{
		if (utils::IsOverlapping(actorShape, m_ShootHitBox))
		{
			if (m_State != EnemyState::Shooting)
			{
				ChangeState(EnemyState::Shooting);
			}
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else if (utils::IsOverlapping(actorShape, m_DetectionBox))
		{
			if (m_State != EnemyState::Running)
			{
				ChangeState(EnemyState::Running);
			}
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else
			if (m_State != EnemyState::Holding)
			{
				ChangeState(EnemyState::Holding);
			}
	}
	else
	{
		if (!m_Dead)
		{
			m_pJackalDeath->Play(0);
			m_Dead = true;
		}
	}
}