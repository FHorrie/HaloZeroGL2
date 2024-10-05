#pragma once
#include "StaticTextures.h"

class Texture;

class StaticSprite
{
public:
	StaticSprite(const std::string& screenType, const StaticTextures& textures, const Point2f& startLocation);
	virtual ~StaticSprite() = default;

	virtual void Draw() const;

	virtual void SetLocation(Point2f newLocation);

protected:
	Point2f m_Position;
	const Texture* m_pSpriteTexture;
};

