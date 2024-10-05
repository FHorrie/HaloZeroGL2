#include "pch.h"
#include "CasingSprite.h"
#include <iostream>

CasingSprite::CasingSprite(const StaticTextures& textures, const Point2f& startLocation, float cosAngle, float sinAngle, bool isFlipped)
	:AnimatedSprite{ "Casing", textures, startLocation, 3, 20, 1, 3 },
	m_HitBox{ startLocation.x, startLocation.y, 9, 7 },
	m_DirVelocity{},
	m_Acceleration{ 0, -1200 },
	m_IsFlipped{ isFlipped }
{
	if (!m_IsFlipped)
	{
		m_DirVelocity = Vector2f((400 * cosAngle), (300 - 300 * sinAngle));
		m_HitBox.left -= 60 * sinAngle / 2;
	}

	else
	{
		m_DirVelocity = Vector2f((400 * cosAngle), (300 + 300 * sinAngle));
		m_HitBox.left -= 60 * sinAngle / 2;
	}
}

void CasingSprite::Update(float elapsedSec, const Level& level)
{
	UpdateCurrentFrame(elapsedSec);
	UpdatePosition(elapsedSec, level);
}

void CasingSprite::UpdateCurrentFrame(float elapsedSec)
{
	m_AccuTime += elapsedSec;

	if (m_AccuTime >= (1.f / m_Framerate))
	{
		m_AccuTime -= 1.f / m_Framerate;

		(m_CurrentFrame < (m_nFrames - 1)) ? m_CurrentFrame++ : m_CurrentFrame = 0;
	}
}

void CasingSprite::UpdatePosition(float elapsedSec, const Level& level)
{
	(!level.IsOnGround(m_HitBox, m_DirVelocity)) ? m_DirVelocity.y += m_Acceleration.y * elapsedSec : m_NeedsDeletion = true;

	(!m_IsFlipped) ? m_HitBox.left -= m_DirVelocity.x * elapsedSec : m_HitBox.left += m_DirVelocity.x * elapsedSec;

	m_HitBox.bottom += m_DirVelocity.y * elapsedSec;

	m_Position = Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + m_HitBox.height / 2);
	level.HandleCollision(m_HitBox, m_DirVelocity);
}

bool CasingSprite::CheckDeletion()
{
	return (m_NeedsDeletion) ? true : false;
}