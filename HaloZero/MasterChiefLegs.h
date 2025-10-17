#pragma once
#include "AnimatedSprite.h"

class MasterChiefBase;

class MasterChiefLegs final : public AnimatedSprite
{
public:
	explicit MasterChiefLegs(const StaticTextures& textures, 
		const Point2f& startLocation, unsigned short framerate, MasterChiefBase* const base);
	MasterChiefLegs(const MasterChiefLegs& other) = delete;
	MasterChiefLegs& operator= (const MasterChiefLegs& other) = delete;
	MasterChiefLegs(MasterChiefLegs&& other) = delete;
	MasterChiefLegs& operator=(MasterChiefLegs&& other) = delete;
	~MasterChiefLegs() = default;

	void Draw() const override;
	void Update(float elapsedSec);

private:
	unsigned short m_CurrentRow{};

	MasterChiefBase* const m_BasePtr{};

	void UpdateFramesState();
	void UpdateCurrentFrame(float elapsedSec);
};

