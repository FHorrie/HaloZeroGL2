#include "pch.h"
#include "ImpNeedle.h"
#include "Texture.h"

ImpNeedle::ImpNeedle(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation)
	:EnemyBase("ImpNeedle", textures, sounds, startLocation, 8, 4, 8, 30, 100.f, EnemyGunType::needle)
	,m_pImpDeath{sounds.GetSoundEffect("ImpDeath")}
{
}

void ImpNeedle::Update(float elapsedSec, const Level& level, const Rectf& actorShape)
{
	StateSwitch(actorShape, elapsedSec);
	HandleShootHitBox(150);
	HandleDetectionBox(700);
	UpdatePosition(elapsedSec, level, 300.f);
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);
}

void ImpNeedle::UpdateFramesState()
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
		m_nFrames = 8;
		m_CurrentRow = 1;
		m_Looped = true;
		break;
	case EnemyState::grenade:
		m_nFrames = 2;
		m_CurrentRow = 2;
		m_Looped = false;
		break;
	case EnemyState::dead:
		m_nFrames = 4;
		m_CurrentRow = 3;
		m_Looped = false;
		break;
	}
}

void ImpNeedle::StateSwitch(const Rectf& actorShape, float elapsedSec)
{
	if (m_State != EnemyState::dead)
	{
		if (utils::IsOverlapping(actorShape, m_ShootHitBox))
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
		if(!m_Dead)
		{
			m_pImpDeath->Play(0);
			m_Dead = true;
		}
	}
}
