#pragma once
#include "EnemyBase.h"

class ImpPistol final : public EnemyBase
{
public:
	explicit ImpPistol(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation);
	ImpPistol(const ImpPistol& other) = delete;
	ImpPistol& operator= (const ImpPistol& other) = delete;
	~ImpPistol() = default;

	void Update(float elapsedSec, const Level& level, const Rectf& actorShape) override;

	void StateSwitch(const Rectf& actorShape, float elapsedSec) override;

private:
	void UpdateFramesState() override;

	const SoundEffect* m_pImpDeath;
	
};