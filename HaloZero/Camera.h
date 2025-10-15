#pragma once
#include "utils.h"

class Camera final
{
public:
	explicit Camera();
	explicit Camera(float width, float height);
	Camera(const Camera& other) = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& other) = delete;
	~Camera();

	void SetLevelBoundaries(const Rectf& levelBoundaries);
	Point2f CameraPos(const Rectf& target) const;
	
private:
	const float m_Width{ 360 };
	const float m_Height{ 240 };
	Rectf m_LevelBoundaries{};

	Point2f Track(const Rectf& target) const;
	void Clamp(Point2f& bottomLeftPos) const;
};