#include "pch.h"
#include "debugpch.h"
#include "MasterChiefTorso.h"
#include "MasterChiefBase.h"
#include "CasingSprite.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

MasterChiefTorso::MasterChiefTorso(const StaticTextures& textures, 
	const Point2f& startLocation, unsigned short framerate, MasterChiefBase* const base)
	: AnimatedSprite("MasterChiefTorso", textures, startLocation, 0, framerate, 11, 5)
	, m_BasePtr{ base }
{}

void MasterChiefTorso::Draw() const
{
	const Rectf currentHitbox{ m_BasePtr->GetHitbox() };
	const float currentAngle{ m_BasePtr->GetAngle() };
	const bool flipped{ m_BasePtr->GetFlipped() };
	const bool secondaryEquipped{ m_BasePtr->GetFlipped() };

	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight() / m_Rows;
	srcRect.left = (m_CurrentFrame % (m_Cols + 1)) * srcRect.width;
	srcRect.bottom = srcRect.height * (m_CurrentRow * 2 + m_RowOffset + 1);

	Rectf destRect{};
	destRect.left = -currentHitbox.width/2;
	destRect.bottom = 0;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	const float offsetAmount{ 5.f };
	const float xOffset{ flipped ? currentHitbox.Center().x - offsetAmount : currentHitbox.Center().x + offsetAmount };	
	glPushMatrix();
	glTranslatef(xOffset, currentHitbox.bottom + currentHitbox.height - 50, 0);
	glRotatef(currentAngle / float(M_PI) * 180.f, 0, 0, 1);
	if (flipped)
	{
		glScalef(-1, 1, 1);
	}
	m_pSpriteTexture->Draw(destRect, srcRect);
	glPopMatrix();
}

void MasterChiefTorso::Update(float elapsedSec)
{
	UpdateFramesState();
	UpdateCurrentFrame(elapsedSec);
}

void MasterChiefTorso::UpdateFramesState()
{
	Rectf currentHitbox{ m_BasePtr->GetHitbox() };
	float currentAngle{ m_BasePtr->GetAngle() };
	bool flipped{ m_BasePtr->GetFlipped() };
	bool secondaryEquipped{ m_BasePtr->GetFlipped() };

	switch (m_BasePtr->GetActionState())
	{
	case ActionState::Holding:
		m_nFrames = 1;
		m_CurrentRow = int(m_BasePtr->GetActiveGun());
		m_RowOffset = 0;
		m_Looped = false;
		break;
	case ActionState::Shooting:
		m_nFrames = 2;

		switch (m_BasePtr->GetActiveGun())
		{
		// We set the framerate to match the firerate
		case GunType::SmartRifle:
			m_Framerate = 15;
			break;
		case GunType::MagnumPistol:
			m_Framerate = 8;
			break;
		case GunType::PlasmaPistol:
			m_Framerate = 10;
			break;
		case GunType::PlasmaRifle:
			m_Framerate = 10;
			break;
		case GunType::Needler:
			m_Framerate = 10;
			break;
		default:
			break;
		}
	
		m_CurrentRow = int(m_BasePtr->GetActiveGun());
		m_RowOffset = 0;
		m_Looped = true;
		break;
	case ActionState::Melee:
		m_nFrames = 2;
		m_Framerate = 8;

		m_CurrentRow = int(m_BasePtr->GetActiveGun());
		m_RowOffset = 1;
		m_Looped = false;
		break;
	case ActionState::Grenade:
		m_nFrames = 5;
		m_Framerate = 10;
		m_CurrentRow = 5;
		m_RowOffset = 0;
		m_Looped = false;
		break;
	}
}

void MasterChiefTorso::UpdateCurrentFrame(float elapsedSec)
{
	if (m_nFrames <= 1)
	{
		m_CurrentFrame = 0;
		return;
	}

	m_AccuTime += elapsedSec;

	if (m_AccuTime < 1.f / m_Framerate)
	{
		return;
	}

	if (m_CurrentFrame < (m_nFrames - 1))
	{
		m_CurrentFrame++;
	}
	else
	{
		m_BasePtr->TorsoAnimationLoopCompleted();
		m_CurrentFrame = m_Looped ? 0 : m_nFrames;
	}

	m_AccuTime = 0;
}