#pragma once
#include "AnimatedSprite.h"

class EnvironmentSprite : public AnimatedSprite
{
public:
	explicit EnvironmentSprite(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short frames, unsigned short framerate, unsigned short rows, unsigned short cols, bool looped);
	EnvironmentSprite(const EnvironmentSprite& other) = delete;
	EnvironmentSprite& operator= (const EnvironmentSprite& other) = delete;
	EnvironmentSprite(EnvironmentSprite&& other) = delete;
	EnvironmentSprite& operator=(EnvironmentSprite&& other) = delete;
	~EnvironmentSprite() = default;

	void ResetAnim();
	bool AnimFinished();

private:

};

