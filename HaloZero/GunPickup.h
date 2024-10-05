#pragma once
#include "StaticSprite.h"
#include "Vector2f.h"
#include "Level.h"
#include "MasterChiefBase.h"

class GunPickup final : public StaticSprite
{
public:
	explicit GunPickup(const StaticTextures& textures, const Point2f& startLocation, GunType type);
	explicit GunPickup(const StaticTextures& textures, const Point2f& startLocation, GunType type, int ammo, int reserve);
	GunPickup(const GunPickup& other) = delete;
	GunPickup& operator= (const GunPickup& other) = delete;
	GunPickup(GunPickup&& other) = delete;
	GunPickup& operator=(GunPickup&& other) = delete;
	~GunPickup() = default;

	void Draw() const override;
	void Update(float elapsedSec, const Level& level);

	bool CheckOverlap(const Rectf& actorShape) const;

	GunType GetGunType() const;
	int GetAmmo() const;
	int GetReserve() const;

private:
	const int m_Rows;

	Rectf m_HitBox;

	GunType m_GunType;

	int m_Ammo;
	int m_Reserve;

	Vector2f m_DirVelocity;
	Vector2f m_Acceleration;

	void UpdatePosition(float elapsedSec, const Level& level);

};
