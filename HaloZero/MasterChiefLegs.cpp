#include "pch.h"
#include "debugpch.h"
#include "MasterChiefLegs.h"
#include "MasterChiefBase.h"
#include "Texture.h"
#include "utils.h"

MasterChiefLegs::MasterChiefLegs(const StaticTextures& textures, 
	const Point2f& startLocation, unsigned short framerate, MasterChiefBase* const base)
	: AnimatedSprite("MasterChiefLegs", textures, startLocation, 0, framerate, 4, 9)
	, m_BasePtr{ base }
{
}

void MasterChiefLegs::Draw() const
{
	Rectf currentHitbox{ m_BasePtr->GetHitbox() };

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
	glTranslatef(currentHitbox.left, currentHitbox.bottom, 0);

	if (m_BasePtr->GetFlipped())
	{
		glScalef(-1, 1, 1);
		glTranslatef(-currentHitbox.width, 0, 0);
	}

	m_pSpriteTexture->Draw(destRect, srcRect);
	
	glPopMatrix();

#ifdef _DEBUG_HITBOX
	utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
	utils::DrawRect(currentHitbox);
#endif

}


void MasterChiefLegs::Update(float elapsedSec)
{
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);	
}

void MasterChiefLegs::UpdateFramesState()
{
	switch (m_BasePtr->GetMoveState())
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

		if (m_BasePtr->IsBackwards() == false)
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