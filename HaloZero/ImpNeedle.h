#pragma once
#include "EnemyBase.h"

class ImpNeedle final : public EnemyBase
{
public:
	explicit ImpNeedle(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation);
	ImpNeedle(const ImpNeedle& other) = delete;
	ImpNeedle& operator= (const ImpNeedle& other) = delete;
	~ImpNeedle() = default;

	void Update(float elapsedSec, const Level& level, const Rectf& actorShape) override;

	void StateSwitch(const Rectf& actorShape, float elapsedSec) override;

private:
	void UpdateFramesState() override;

	const SoundEffect* m_pImpDeath;
};
