#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "svgparser.h"
#include "Constants.h"
#include <iostream>

Level::Level() 
	: m_pOutsideTexture{ new Texture("BackGround/BackgroundOutside.png") }
	, m_pBackGroundTexture{ new Texture{ "BackGround/FacilityBack.png" } }
	, m_pForeGroundTexture{ new Texture{ "BackGround/FacilityFront.png"}}
	, m_Boundaries{ 0, 0, m_pBackGroundTexture->GetWidth(), m_pBackGroundTexture->GetHeight() }
	, m_EndShape{11630, 400, 100, 10}
{
	SVGParser::GetVerticesFromSvgFile("BackGround/Facility.svg", m_Vertices);
}

Level::~Level()
{
	delete m_pOutsideTexture;
	delete m_pBackGroundTexture;
	delete m_pForeGroundTexture;
	m_Vertices.~vector();
}

void Level::DrawOutside() const
{
	m_pOutsideTexture->Draw();
}

void Level::DrawBackground() const
{
	m_pBackGroundTexture->Draw();

}

void Level::DrawForeground() const
{
	m_pForeGroundTexture->Draw();
}

void Level::HandleCollision(Rectf& actor, Vector2f& actorVelocity) const
{
	utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });

	Point2f actorCenterTop{ actor.left + actor.width / 2 , actor.bottom + actor.height };
	Point2f actorCenterBottom{ actor.left + actor.width / 2 , actor.bottom };

	// Add a small value to prevent false positives from the floor
	Point2f actorCenterOffsetBottom{ actor.left + actor.width / 2 , actor.bottom + Constants::Epsilon };
	Point2f actorLeftBottom{ actor.left , actor.bottom + Constants::Epsilon };
	Point2f actorRightBottom{ actor.left + actor.width , actor.bottom + Constants::Epsilon };

	utils::HitInfo hitInfo{};

	if (utils::Raycast(m_Vertices[0], actorCenterTop, actorCenterBottom, hitInfo)
		&& IsOnGround(actor, actorVelocity))
	{
		actor.bottom = hitInfo.intersectPoint.y;
		actorVelocity.y = 0;
	}

	if (utils::Raycast(m_Vertices[0], actorLeftBottom, actorCenterOffsetBottom, hitInfo))
	{
		if (IsHittingWallLeft(actor, actorVelocity))
		{
			actor.left = hitInfo.intersectPoint.x;
		}
	}
	else if (utils::Raycast(m_Vertices[0], actorRightBottom, actorCenterOffsetBottom, hitInfo))
	{
		if (IsHittingWallRight(actor, actorVelocity))
		{
			actor.left = hitInfo.intersectPoint.x - actor.width;
		}
	}
}

bool Level::IsOnGround(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	Point2f actorCenterTop{ actorShape.left + actorShape.width / 2 , actorShape.bottom + actorShape.height};
	Point2f actorCenterBottom{ actorShape.left + actorShape.width / 2 , actorShape.bottom - 1};
	utils::HitInfo hitInfo{};

	return utils::Raycast(m_Vertices[0], actorCenterTop, actorCenterBottom);
}

bool Level::IsHittingWallLeft(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	// Raycast in a diagonal manner to prevent slight slope snapping
	Point2f actorLeftBottom{ actorShape.left, actorShape.bottom + actorShape.height / 2 };
	Point2f actorCenterBottom{ actorShape.left + actorShape.width / 2, actorShape.bottom + Constants::Epsilon };
	return utils::Raycast(m_Vertices[0], actorLeftBottom, actorCenterBottom);
}

bool Level::IsHittingWallRight(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	// Raycast in a diagonal manner to prevent slight slope snapping
	Point2f actorRightBottom{ actorShape.left + actorShape.width , actorShape.bottom + actorShape.height / 2 };
	Point2f actorCenterBottom{ actorShape.left + actorShape.width / 2 , actorShape.bottom + Constants::Epsilon };
	return utils::Raycast(m_Vertices[0], actorCenterBottom, actorRightBottom);
}

Rectf Level::GetBoundaries() const
{
	return m_Boundaries;
}

bool Level::HasReachedEnd(const Rectf& actorShape) const
{
	return utils::IsOverlapping(actorShape, m_EndShape);
}