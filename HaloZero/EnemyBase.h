#pragma once
#include "AnimatedSprite.h"
#include "StaticTextures.h"
#include "SoundManager.h"
#include "Level.h"

enum class EnemyState
{
	holding,
	shooting,
	running,
	melee,
	grenade,
	dead
};

enum class EnemyGunType
{
	pistol,
	rifle,
	needle,
	none
};

class EnemyBase : public AnimatedSprite
{
public:
	EnemyBase(const std::string& spriteType, const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation, unsigned short framerate, unsigned short rows, unsigned short cols,
									short startHealth, float speed, EnemyGunType gunType);
	virtual ~EnemyBase() = default;

	void Draw() const override;
	virtual void Update(float elapsedSec, const Level& level, const Rectf& actorShape) = 0;

	Rectf GetShape() const;
	Point2f GetDropPoint() const;

	void TakeDamage(int damage);

	bool IsDead() const;
	bool IsShooting();
	bool IsMelee();

	virtual bool DropWeapon() const;
	EnemyGunType GetWeapon() const;
	bool GetFlipped() const;

	virtual void StateSwitch(const Rectf& actorShape, float elapsedSec) = 0;

protected:
	EnemyState m_State;
	EnemyGunType m_Gun;

	const SoundEffect* m_pPlasmaPistolShot;
	const SoundEffect* m_pPlasmaRifleShot;
	const SoundEffect* m_pNeedlerShot;
	const SoundEffect* m_pHunterShot;

	Rectf m_HitBox;
	Rectf m_DetectionBox;
	Rectf m_ShootHitBox;

	unsigned short m_CurrentRow;

	Vector2f m_DirVelocity;
	Vector2f m_Acceleration;
	const float m_HorSpeed;

	int m_Health;

	bool m_IsFlipped = false;
	bool m_ShotFired = false;
	bool m_Meleeing = false;
	bool m_AnimPlayed = false;

	bool m_Dead = false;

	void ChangeState(EnemyState state);

	virtual void UpdateFramesState() = 0;
	virtual void UpdateCurrentFrame(float elapsedSec);

	void HandleShootHitBox(float boxWidth);
	void HandleDetectionBox(float boxWidth);

	void UpdatePosition(float elapsedSec, const Level& level, float hopForce);

};