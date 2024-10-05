#include "pch.h"
#include "MasterChiefBase.h"

MasterChiefBase::MasterChiefBase(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short framerate, unsigned short rows, unsigned short cols)
	:AnimatedSprite(spriteType, textures, startLocation, 0, framerate, rows, cols),
	m_MoveState{ MoveState::waiting },
	m_ActionState{ ActionState::holding },
	m_JumpBuffer{},
	m_HitBox{startLocation.x, startLocation.y, 70, 104 },
	m_DirVelocity{ 0, 0 },
	m_Acceleration{ 0, -1200 },
	m_HorSpeed{300},
	m_JumpSpeed{600}

{
}

void MasterChiefBase::ChangeMovementState(MoveState state)
{
	m_MoveState = state;
	m_CurrentFrame = 0;
	m_AccuTime = 0;
	
	if (m_MoveState > MoveState::jumping)
		m_HitBox.height = 75;
	else
		m_HitBox.height = 104;
}

void MasterChiefBase::ChangeActionState(ActionState state)
{
	m_ActionState = state;
	m_AccuTime = 0;
	m_CurrentFrame = 0;
}

void MasterChiefBase::UpdatePosition(float elapsedSec, const Level& level)
{
	if (!level.IsOnGround(m_HitBox, m_DirVelocity))
	{
		m_DirVelocity.y += m_Acceleration.y * elapsedSec;
		m_JumpBuffer += elapsedSec;
		if (m_JumpBuffer > 0.15f)
		{
			m_MoveState = MoveState::jumping;
		}
	}
	else
	{
		m_JumpBuffer = 0.f;
		switch (m_MoveState)
		{
		case MoveState::waiting:
			m_DirVelocity = Vector2f{ 0,0 };
			break;
		case MoveState::running:
			m_DirVelocity.x = m_HorSpeed;
			break;
		case MoveState::jumping:
			m_DirVelocity.y = m_JumpSpeed;
			break;
		case MoveState::waitingCrouch:
			m_DirVelocity = Vector2f{ 0,0 };
			break;
		case MoveState::runningCrouch:
			m_DirVelocity.x = m_HorSpeed / 2.f;
			break;
		default:
			break;
		}
	}
	if (!m_IsFlipped)
		(!m_IsBackwards) ? m_HitBox.left += m_DirVelocity.x * elapsedSec : m_HitBox.left -= m_DirVelocity.x * elapsedSec;
	else
		(m_IsBackwards) ? m_HitBox.left += m_DirVelocity.x * elapsedSec : m_HitBox.left -= m_DirVelocity.x * elapsedSec;
	
	m_HitBox.bottom += m_DirVelocity.y * elapsedSec;

	level.HandleCollision(m_HitBox, m_DirVelocity);
	
	
}

Rectf MasterChiefBase::GetShape() const
{
	return m_HitBox;
}

Point2f MasterChiefBase::GetDropPoint() const
{
	return Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 2 * m_HitBox.height / 3);
}