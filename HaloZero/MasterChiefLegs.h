#pragma once
#include "MasterChiefBase.h"

class MasterChiefLegs final : public MasterChiefBase
{
public:
	explicit MasterChiefLegs(const StaticTextures& textures, const Point2f& startLocation, unsigned short framerate);
	MasterChiefLegs(const MasterChiefLegs& other) = delete;
	MasterChiefLegs& operator= (const MasterChiefLegs& other) = delete;
	MasterChiefLegs(MasterChiefLegs&& other) = delete;
	MasterChiefLegs& operator=(MasterChiefLegs&& other) = delete;
	~MasterChiefLegs() = default;


	void Draw() const override;
	void Update(float elapsedSec, const Level& level);

	void SetFlipped(bool flipped);

private:
	unsigned short m_CurrentRow;

	void InputStateMovement();
	void UpdateFramesState();
	void UpdateCurrentFrame(float elapsedSec);
};

