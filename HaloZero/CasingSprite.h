#pragma once
#include "AnimatedSprite.h"
#include "Level.h"

class CasingSprite final : public AnimatedSprite
{
public:
	explicit CasingSprite(const StaticTextures& textures, const Point2f& startLocation, float cosAngle, float sinAngle, bool isFlipped);
	CasingSprite(const CasingSprite& other) = delete;
	CasingSprite& operator= (const CasingSprite& other) = delete;
	CasingSprite(CasingSprite&& other) = delete;
	CasingSprite& operator=(CasingSprite&& other) = delete;
	~CasingSprite() = default;

	void Update(float elapsedSec, const Level& level);

	bool CheckDeletion();

private:
	Rectf m_HitBox;

	Vector2f m_DirVelocity;
	Vector2f m_Acceleration;

	bool m_IsFlipped;
	bool m_NeedsDeletion = false;

	void UpdateCurrentFrame(float elapsedSec);
	void UpdatePosition(float elapsedSec, const Level& level);

};