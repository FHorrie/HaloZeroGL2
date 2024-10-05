#include "pch.h"
#include "Camera.h"
#include "Level.h"

Camera::Camera()
	:m_Width{360},
	m_Height{240},
	m_LevelBoundaries{}
	
{
}

Camera::Camera(float width, float height)
	:m_Width{ width },
	m_Height{ height },
	m_LevelBoundaries{}
{
}

Camera::~Camera()
{
}

void Camera::SetLevelBoundaries(const Rectf& levelBoundaries)
{
	m_LevelBoundaries = levelBoundaries;
}

Point2f Camera::CameraPos(const Rectf& target) const
{
	Point2f bottomLeftPos{ Track(target) - Point2f(m_Width/2 , m_Height/2) };
	Clamp(bottomLeftPos);
	return bottomLeftPos;
}

Point2f Camera::Track(const Rectf& target) const
{
	return Point2f{ target.left + target.width, target.bottom + target.height / 2 };
}

void Camera::Clamp(Point2f& bottomLeftPos) const
{
	bool camHitLeft{ bottomLeftPos.x <= m_LevelBoundaries.left };
	bool camHitRight{ bottomLeftPos.x >= m_LevelBoundaries.left + m_LevelBoundaries.width - m_Width };

	if (camHitLeft || camHitRight)
	{
		(camHitLeft) ? bottomLeftPos.x = m_LevelBoundaries.left : bottomLeftPos.x = m_LevelBoundaries.left + m_LevelBoundaries.width - m_Width;
	}

	bool camHitBottom{ bottomLeftPos.y <= m_LevelBoundaries.bottom };
	bool camHitTop{ bottomLeftPos.y >= m_LevelBoundaries.bottom + m_LevelBoundaries.height - m_Height };

	if (camHitBottom || camHitTop)
	{
		(camHitBottom) ? bottomLeftPos.y = m_LevelBoundaries.bottom : bottomLeftPos.y = m_LevelBoundaries.bottom + m_LevelBoundaries.height - m_Height;
	}
}