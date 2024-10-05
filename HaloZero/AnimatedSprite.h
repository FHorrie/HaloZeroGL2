#pragma once
#include "StaticSprite.h"

class AnimatedSprite : public StaticSprite
{
public:
	AnimatedSprite(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short frames, unsigned short framerate, unsigned short rows, unsigned short cols);
	virtual ~AnimatedSprite() = default;

	virtual void Draw() const override;
	void Update(float elapsedSec);

protected:
	unsigned short m_nFrames;
	unsigned short m_Framerate;
	const unsigned short m_Rows;
	const unsigned short m_Cols;
	unsigned short m_CurrentFrame = 0;
	float m_AccuTime = 0;
	bool m_Looped = true;
};

