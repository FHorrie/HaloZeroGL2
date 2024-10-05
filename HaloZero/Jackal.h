#pragma once
#include "EnemyBase.h"

class Jackal final : public EnemyBase
{
public:
	explicit Jackal(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation);
	Jackal(const Jackal& other) = delete;
	Jackal& operator= (const Jackal& other) = delete;
	~Jackal() = default;

	void Update(float elapsedSec, const Level& level, const Rectf& actorShape) override;

	void StateSwitch(const Rectf& actorShape, float elapsedSec) override;

private:
	void UpdateFramesState() override;

	const SoundEffect* m_pJackalDeath;
};

