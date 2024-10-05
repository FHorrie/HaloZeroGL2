#pragma once
#include "EnemyBase.h"

class EliteRifle final : public EnemyBase
{
public:
	explicit EliteRifle(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation);
	EliteRifle(const EliteRifle& other) = delete;
	EliteRifle& operator= (const EliteRifle& other) = delete;
	EliteRifle(EliteRifle&& other) = delete;
	EliteRifle& operator=(EliteRifle&& other) = delete;
	~EliteRifle() = default;

	void Update(float elapsedSec, const Level& level, const Rectf& actorShape) override;

	void StateSwitch(const Rectf& actorShape, float elapsedSec) override;

private:
	void UpdateCurrentFrame(float elapsedSec) override;
	void UpdateFramesState() override;

	const SoundEffect* m_pEliteDeath;

	float m_MeleeAccuTime;
	static const float MELEETIME;
};

