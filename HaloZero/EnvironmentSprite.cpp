#include "pch.h"
#include "EnvironmentSprite.h"

EnvironmentSprite::EnvironmentSprite(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short frames, unsigned short framerate, unsigned short rows, unsigned short cols, bool looped)
	:AnimatedSprite(spriteType, textures, startLocation, frames, framerate, rows, cols)
{
	m_Looped = looped;
}

void EnvironmentSprite::ResetAnim()
{
	m_CurrentFrame = 0;
}

bool EnvironmentSprite::AnimFinished()
{
	return (m_CurrentFrame == m_nFrames) ? true : false;
}