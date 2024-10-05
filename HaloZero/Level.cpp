#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "svgparser.h"
#include <iostream>

Level::Level() :
	m_pOutsideTexture{ new Texture("BackGround/BackgroundOutside.png") },
	m_pBackGroundTexture{ new Texture{ "BackGround/FacilityBack.png" } },
	m_pForeGroundTexture{ new Texture{ "BackGround/FacilityFront.png"}},
	m_Boundaries{ 0, 0, m_pBackGroundTexture->GetWidth(), m_pBackGroundTexture->GetHeight() },
	m_EndShape{11630, 400, 100, 10}
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
	Point2f actorP1{ actor.left + actor.width / 2 , actor.bottom + actor.height };
	Point2f actorP2{ actor.left + actor.width / 2 , actor.bottom };
	Point2f actorP3{ actor.left , actor.bottom + actor.height / 6 };
	Point2f actorP4{ actor.left + actor.width/2 , actor.bottom + actor.height / 6 };
	Point2f actorP5{ actor.left + actor.width , actor.bottom + actor.height / 6 };
	utils::HitInfo hitInfo1{};
	utils::HitInfo hitInfo2{};
	utils::HitInfo hitInfo3{};
	if (utils::Raycast(m_Vertices[0], actorP1, actorP2, hitInfo1))
	{
		if (IsOnGround(actor, actorVelocity))
		{
			actor.bottom = hitInfo1.intersectPoint.y;
			actorVelocity.y = 0;
		}
	}
	if (utils::Raycast(m_Vertices[0], actorP3, actorP4, hitInfo2))
	{
		if (IsHittingWallLeft(actor, actorVelocity))
		{
			actor.left = hitInfo2.intersectPoint.x;
			//actorVelocity.x = 0;
		}
		return;
	}
	if (utils::Raycast(m_Vertices[0], actorP4, actorP5, hitInfo3))
	{
		if (IsHittingWallRight(actor, actorVelocity))
		{
			actor.left = hitInfo3.intersectPoint.x - actor.width;
			//actorVelocity.x = 0;
		}
		return;
	}
	
}

bool Level::IsOnGround(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	Point2f actorP1{ actorShape.left + actorShape.width / 2 , actorShape.bottom + actorShape.height};
	Point2f actorP2{ actorShape.left + actorShape.width / 2 , actorShape.bottom - 1 };
	utils::HitInfo hitInfo{};

	return (utils::Raycast(m_Vertices[0], actorP1, actorP2, hitInfo)) ? true : false;
}

bool Level::IsHittingWallLeft(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	Point2f actorP3{ actorShape.left , actorShape.bottom + actorShape.height / 6 };
	Point2f actorP4{ actorShape.left + actorShape.width / 2 , actorShape.bottom + actorShape.height / 6 };
	utils::HitInfo hitInfo{};
	if (utils::Raycast(m_Vertices[0], actorP3, actorP4, hitInfo))
	{
		//std::cout << hitInfo.intersectPoint.x << "\n";
		return true;
	}
	else
		return false;
}

bool Level::IsHittingWallRight(const Rectf& actorShape, const Vector2f& actorVelocity) const
{
	Point2f actorP4{ actorShape.left + actorShape.width / 2 , actorShape.bottom + actorShape.height / 6 };
	Point2f actorP5{ actorShape.left + actorShape.width , actorShape.bottom + actorShape.height / 6 };
	utils::HitInfo hitInfo{};
	if (utils::Raycast(m_Vertices[0], actorP4, actorP5, hitInfo))
		return true;
	else
		return false;
}

Rectf Level::GetBoundaries() const
{
	return m_Boundaries;
}

bool Level::HasReachedEnd(const Rectf& actorShape) const
{
	if (utils::IsOverlapping(actorShape, m_EndShape))
		return true;
	else return false;
}