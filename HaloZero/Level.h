#pragma once
#include <vector>
#include "utils.h"

class Texture;
class Platform;

class Level final
{
public:
	explicit Level();
	Level(const Level& other) = default;
	Level& operator= (const Level& other) = default;
	Level(Level && other) = delete;
	Level& operator=(Level&& other) = delete;
	~Level();

	void DrawOutside() const;
	void DrawBackground() const;
	void DrawForeground() const;

	void HandleCollision(Rectf& actor, Vector2f& actorVelocity) const;
	bool IsOnGround(const Rectf& actorShape, const Vector2f& actorVelocity) const;
	bool IsHittingWallLeft(const Rectf& actorShape, const Vector2f& actorVelocity) const;
	bool IsHittingWallRight(const Rectf& actorShape, const Vector2f& actorVelocity) const;
	
	Rectf GetBoundaries() const;

	bool HasReachedEnd(const Rectf& actorShape) const;

private:
	std::vector<std::vector<Point2f>> m_Vertices;
	Texture* m_pOutsideTexture;
	Texture* m_pBackGroundTexture;
	Texture* m_pForeGroundTexture;
	const Rectf m_Boundaries;
	const Rectf m_EndShape;
};

