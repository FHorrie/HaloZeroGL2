#include "pch.h"
#include "debugpch.h"
#include "MasterChiefLegs.h"
#include "Texture.h"
#include "utils.h"

MasterChiefLegs::MasterChiefLegs(const StaticTextures& textures, const Point2f& startLocation, unsigned short framerate)
	:MasterChiefBase("MasterChiefLegs", textures, startLocation, framerate, 4, 9),
	m_CurrentRow{}
{
}

void MasterChiefLegs::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight() / m_Rows;
	srcRect.left = (m_CurrentFrame % m_Cols) * srcRect.width;
	srcRect.bottom = srcRect.height * (m_CurrentRow + 1);

	Rectf destRect{};
	destRect.left = 0;
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
		utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
		utils::DrawRect(m_HitBox);
#endif
}


void MasterChiefLegs::Update(float elapsedSec, const Level& level)
{
	UpdateFramesState();
	InputStateMovement();
	UpdatePosition(elapsedSec, level);
	UpdateCurrentFrame(elapsedSec);
	
}

void MasterChiefLegs::InputStateMovement()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	//Movement states
	if (pStates[SDL_SCANCODE_SPACE])
	{
		if (m_MoveState != MoveState::Jumping && m_MoveState != MoveState::WaitingCrouch && m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::Jumping);
	}
	else if (pStates[SDL_SCANCODE_D] && pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::RunningCrouch);
		m_IsBackwards = m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_A] && pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::RunningCrouch);
		m_IsBackwards = !m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::WaitingCrouch)
			ChangeMovementState(MoveState::WaitingCrouch);
	}
	else if (pStates[SDL_SCANCODE_D])
	{
		if (m_MoveState != MoveState::Running)
			ChangeMovementState(MoveState::Running);
		m_IsBackwards = m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_A])
	{
		if (m_MoveState != MoveState::Running)
			ChangeMovementState(MoveState::Running);
		m_IsBackwards = !m_IsFlipped;
	}
	else
	{
		if (m_MoveState != MoveState::Waiting)
			ChangeMovementState(MoveState::Waiting);
	}
}

void MasterChiefLegs::UpdateFramesState()
{

	switch (m_MoveState)
	{
	case MoveState::Waiting:
		m_nFrames = 1;
		m_CurrentRow = 0;
		m_Looped = false;
		break;
	case MoveState::Running:
		m_nFrames = 9;
		m_CurrentRow = 1;
		m_Looped = true;
		break;
	case MoveState::Jumping:
		m_nFrames = 5;
		m_CurrentRow = 2;
		m_Looped = false;
		break;
	case MoveState::WaitingCrouch:
		m_nFrames = 1;
		m_CurrentRow = 3;
		m_Looped = false;
		break;
	case MoveState::RunningCrouch:
		m_nFrames = 4;
		m_CurrentRow = 3;
		m_Looped = true;
		break;
	}
}

void MasterChiefLegs::UpdateCurrentFrame(float elapsedSec)
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

		if (!m_IsBackwards)
		{
			if (m_CurrentFrame < (m_nFrames - 1))
			{
				m_CurrentFrame++;
			}
			else
			{
				if (m_Looped) m_CurrentFrame = 0;
				else;
			}
		}
		else
		{
			if (m_CurrentFrame >= 1)
			{
				m_CurrentFrame--;
			}
			else
			{
				if (m_Looped) m_CurrentFrame = m_nFrames - 1;
				else;
			}
		}

	}
}

void MasterChiefLegs::SetFlipped(bool flipped)
{
	m_IsFlipped = flipped;
}