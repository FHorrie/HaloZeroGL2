#include "pch.h"
#include "StaticSprite.h"
#include "Texture.h"
#include "StaticTextures.h"

StaticSprite::StaticSprite(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation)
	: m_Position(startLocation)
{
	m_pSpriteTexture = textures.GetTexture(spriteType);
}

void StaticSprite::Draw() const
{
	m_pSpriteTexture->Draw(Rectf(m_Position.x, m_Position.y, m_pSpriteTexture->GetWidth(), m_pSpriteTexture->GetHeight()));
}

void StaticSprite::SetLocation(Point2f newLocation)
{
	m_Position = newLocation;
}