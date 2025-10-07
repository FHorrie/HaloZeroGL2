#include "pch.h"
#include "MasterChiefBase.h"

MasterChiefBase::MasterChiefBase(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short framerate, unsigned short rows, unsigned short cols)
	:AnimatedSprite(spriteType, textures, startLocation, 0, framerate, rows, cols),
	m_HitBox{startLocation.x, startLocation.y, 70, 104 },
	m_Velocity{ 0, 0 },
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
	
	if (m_MoveState > MoveState::Jumping)
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
	if (!level.IsOnGround(m_HitBox, m_Velocity))
	{
		m_Velocity.y += m_Acceleration.y * elapsedSec;
		m_JumpBuffer += elapsedSec;
		if (m_JumpBuffer > 0.15f)
		{
			m_MoveState = MoveState::Jumping;
		}
	}
	else
	{
		m_JumpBuffer = 0.f;
		switch (m_MoveState)
		{
		case MoveState::Waiting:
			m_Velocity = Vector2f{0.f, 0.f};
			break;
		case MoveState::Running:
			m_Velocity.x = m_HorSpeed;
			break;
		case MoveState::Jumping:
			m_Velocity.y = m_JumpSpeed;
			break;
		case MoveState::WaitingCrouch:
			m_Velocity = Vector2f{ 0.f, 0.f };
			break;
		case MoveState::RunningCrouch:
			m_Velocity.x = m_HorSpeed / 2.f;
			break;
		default:
			break;
		}
	}
	if (m_IsFlipped == false)
		m_IsBackwards == false ? m_HitBox.left += m_Velocity.x * elapsedSec : m_HitBox.left -= m_Velocity.x * elapsedSec;
	else
		m_IsBackwards ? m_HitBox.left += m_Velocity.x * elapsedSec : m_HitBox.left -= m_Velocity.x * elapsedSec;
	
	m_HitBox.bottom += m_Velocity.y * elapsedSec;

	level.HandleCollision(m_HitBox, m_Velocity);
	
	
}

Rectf MasterChiefBase::GetShape() const
{
	return m_HitBox;
}

Point2f MasterChiefBase::GetDropPoint() const
{
	return Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 2 * m_HitBox.height / 3);
}