#include "pch.h"
#include "AnimatedSprite.h"
#include "Texture.h"

AnimatedSprite::AnimatedSprite(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short frames, unsigned short framerate, unsigned short rows, unsigned short cols)
	:StaticSprite(spriteType, textures, startLocation),
	m_nFrames{ frames },
	m_Framerate{ framerate },
	m_Rows{ rows },
	m_Cols{ cols }
{
}

void AnimatedSprite::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight() / ((m_Cols * m_Rows) / m_Cols);
	srcRect.left = (m_CurrentFrame % m_Cols) * srcRect.width;
	srcRect.bottom = srcRect.height * ((m_CurrentFrame / m_Cols));

	Rectf destRect{};
	destRect.left = m_Position.x;
	destRect.bottom = m_Position.y;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	m_pSpriteTexture->Draw(destRect, srcRect);
}

void AnimatedSprite::Update(float elapsedSec)
{
	m_AccuTime += elapsedSec;
	if (m_AccuTime >= (1.f / m_Framerate))
	{
		m_AccuTime -= 1.f / m_Framerate;

		if (m_Looped) (m_CurrentFrame < m_nFrames) ? m_CurrentFrame++ : m_CurrentFrame = 0;
		else (m_CurrentFrame < m_nFrames) ? m_CurrentFrame++ : m_CurrentFrame = m_nFrames;
	}
}