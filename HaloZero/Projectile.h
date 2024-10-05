#pragma once
#include "AnimatedSprite.h"
#include "Vector2f.h"
#include "Level.h"
#include "MasterChiefBase.h"
#include "EnemyBase.h"

class Projectile final : public StaticSprite
{
public:
	explicit Projectile(const StaticTextures& textures, const Point2f& startLocation, float angle, GunType type, bool isFlipped);
	explicit Projectile(const StaticTextures& textures, const Point2f& startLocation, float angle, EnemyGunType enemyType, bool enemyFlipped);
	Projectile(const Projectile& other) = delete;
	Projectile& operator= (const Projectile& other) = delete;
	Projectile(Projectile&& other) = delete;
	Projectile& operator=(Projectile&& other) = delete;
	~Projectile() = default;

	void Draw() const;

	void UpdatePosition(float elapsedSec, const Level& level);
	bool CheckHit(const Rectf& actorShape);

	void CheckInScreen(const Rectf& WindowRect);
	bool CheckDeletion() const;
	int GetDamage() const;
	Rectf GetHitBox() const;

private:
	short m_BulletType;
	bool m_DrawBullet;
	int m_Damage;

	short m_Cols;
	
	float m_Angle;

	Rectf m_HitBox;

	float m_BulletSpeed;
	Vector2f m_DirVelocity;
	bool m_NeedsDeletion = false;

	bool m_IsFlipped;
};