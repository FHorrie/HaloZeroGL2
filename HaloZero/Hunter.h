#pragma once
#include "EnemyBase.h"

class Hunter final : public EnemyBase
{
public:
	explicit Hunter(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation);
	Hunter(const Hunter& other) = delete;
	Hunter& operator= (const Hunter& other) = delete;
	~Hunter() = default;

	void Update(float elapsedSec, const Level& level, const Rectf& actorShape) override;

	void StateSwitch(const Rectf& actorShape, float elapsedSec) override;

	bool DropWeapon() const override;

private:
	void UpdateCurrentFrame(float elapsedSec) override;
	void UpdateFramesState() override;

	const SoundEffect* m_pHunterDeath;

	float m_MeleeAccuTime;
	static const float MELEETIME;

	float m_ShootAccuTime;
	static const float SHOOTTIME;
	
	bool m_StopMov = false;

};
