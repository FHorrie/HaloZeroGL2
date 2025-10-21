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

	bool NeedsDeletion() const { return m_NeedsDeletion; }
	int GetDamage() const { return m_Damage; }
	Rectf GetHitBox() const { return m_HitBox; }

private:
	bool m_DrawBullet{};
	bool m_NeedsDeletion{};
	bool m_IsFlipped{};
	
	short m_BulletType{};
	short m_Cols{};
	float m_Angle{};
	
	int m_Damage{};
	float m_BulletSpeed{};

	Vector2f m_DirVelocity{};
	Rectf m_HitBox{};
};