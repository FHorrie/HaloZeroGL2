#include "pch.h"
#include "Hunter.h"
#include "Texture.h"

const float Hunter::MELEETIME{ 1.5f };
const float Hunter::SHOOTTIME{ 1.f };

Hunter::Hunter(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation)
	:EnemyBase("Hunter", textures, sounds, startLocation, 10, 4, 6, 200, 100.f, EnemyGunType::none)
	, m_pHunterDeath{sounds.GetSoundEffect("HunterDeath")}
	, m_MeleeAccuTime{}
	, m_ShootAccuTime{}
{
}

void Hunter::Update(float elapsedSec, const Level& level, const Rectf& actorShape)
{
	StateSwitch(actorShape, elapsedSec);
	HandleShootHitBox(200);
	HandleDetectionBox(700);
	UpdatePosition(elapsedSec, level, 50.f);
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);
}

void Hunter::UpdateCurrentFrame(float elapsedSec)
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
			if (m_Looped) m_CurrentFrame = 0;

			if (m_State == EnemyState::shooting)
			{
				m_ShootAccuTime -= SHOOTTIME;
				m_ShotFired = true;
				ChangeState(EnemyState::holding);
			}
			else if (m_State == EnemyState::melee)
			{
				m_Meleeing = true;
				m_MeleeAccuTime -= MELEETIME;
				ChangeState(EnemyState::holding);
			}
			else;
		}
	}
}

void Hunter::UpdateFramesState()
{
	switch (m_State)
	{
	case EnemyState::holding:
		m_nFrames = 1;
		m_CurrentRow = 0;
		m_Looped = false;
		break;
	case EnemyState::shooting:
		m_nFrames = 3;
		m_CurrentRow = 0;
		m_Looped = true;
		break;
	case EnemyState::melee:
		m_nFrames = 2;
		m_CurrentRow = 1;
		m_Looped = false;
		break;
	case EnemyState::running:
		m_nFrames = 6;
		m_CurrentRow = 2;
		m_Looped = true;
		break;
	case EnemyState::dead:
		m_nFrames = 4;
		m_CurrentRow = 3;
		m_Looped = false;
		break;
	}
}

void Hunter::StateSwitch(const Rectf& actorShape, float elapsedSec)
{
	if (m_State != EnemyState::dead)
	{
		if (utils::IsOverlapping(actorShape, m_HitBox))
		{
			m_MeleeAccuTime += elapsedSec;
			if (m_State != EnemyState::melee && m_MeleeAccuTime >= MELEETIME)
			{
				ChangeState(EnemyState::melee);
			}
			if (actorShape.left + actorShape.width/2 <= m_HitBox.left + m_HitBox.width/2)
				m_IsFlipped = false;
			else
				m_IsFlipped = true;
		}
		else if (utils::IsOverlapping(actorShape, m_ShootHitBox))
		{
			if (!m_StopMov)
			{
				ChangeState(EnemyState::holding);
				m_StopMov = true;
			}

			m_ShootAccuTime += elapsedSec;
			if (m_State != EnemyState::shooting && m_ShootAccuTime >= SHOOTTIME)
				ChangeState(EnemyState::shooting);
			
			if (actorShape.left + actorShape.width / 2 <= m_HitBox.left + m_HitBox.width / 2)
				m_IsFlipped = false;

			else
				m_IsFlipped = true;
		}
		else if (utils::IsOverlapping(actorShape, m_DetectionBox))
		{
			m_StopMov = false;
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
				if (m_StopMov) m_StopMov = false;
				ChangeState(EnemyState::holding);
			}
	}
	else
	{
		if (!m_Dead)
		{
			m_pHunterDeath->Play(0);
			m_Dead = true;
		}
	}
}

bool Hunter::DropWeapon() const
{
	return false;
}