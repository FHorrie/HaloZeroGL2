#pragma once
#include "MasterChiefBase.h"
#include "StaticTextures.h"
#include "SoundManager.h"

class Texture;
class CasingSprite;
class Projectile;

class MasterChiefTorso final : public MasterChiefBase
{
public:
	explicit MasterChiefTorso(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation, unsigned short framerate, const Rectf& window);
	MasterChiefTorso(const MasterChiefTorso& other) = delete;
	MasterChiefTorso& operator= (const MasterChiefTorso& other) = delete;
	MasterChiefTorso(MasterChiefTorso&& other) = delete;
	MasterChiefTorso& operator=(MasterChiefTorso&& other) = delete;
	~MasterChiefTorso();

	void Draw() const override;
	void Update(float elapsedSec, const Level& level, const Rectf& hitbox, const StaticTextures& textures);

	void SaveMousePos(const Point2f& mousePos);
	void AdjustMousePos(const Point2f& movement);

	void DrawCrossHair() const;
	
	void HandleFire(bool activate);
	void HandleMelee();
	void TakeDamage(int damage);

	bool GetFlipped() const;
	bool IsShooting();
	bool IsMelee();
	void PlayMeleeSound();

	void ChangeGun(GunType type, int ammo, int reserve);
	void AddReserve(int reserve, GunType type);

	GunType GetActiveGun() const;
	GunType GetPrimary() const;
	GunType GetSecondary() const;

	float GetAngle() const;

	int GetAmmo() const;
	int GetReserve() const;

	float GetShield(bool max) const;
	float GetHealth(bool max) const;
	bool IsDead() const;

private:

	const float m_MaxShield{ 150 };
	float m_Shield{ m_MaxShield };
	const float m_MaxHealth{ 70 };
	float m_Health{ m_MaxHealth };
	bool m_Dead{ false };

	float m_RegenAccuTime;
	const float m_MaxRegenTime;
	float m_ShieldAccuTime;
	const float m_MaxShieldTime;

	int m_PrimaryAmmo;
	int m_PrimaryReserve;

	int m_SecondaryAmmo;
	int m_SecondaryReserve;

	float m_PrimaryReloadTime;
	float m_SecondaryReloadTime;

	GunType m_FirstSlot;
	GunType m_SecondSlot;
	bool m_IsSecondaryEquipped = false;

	const SoundEffect* m_pSmartRifleShot;
	const SoundEffect* m_pMagnumShot;
	const SoundEffect* m_pPlasmaPistolShot;
	const SoundEffect* m_pPlasmaRifleShot;
	const SoundEffect* m_pNeedlerShot;
	const SoundEffect* m_pMeleeSound;

	float m_ShieldBeepTime;
	bool m_RechargePlayed = false;

	const SoundEffect* m_pShieldBeep;
	const SoundEffect* m_pShieldRecharge;

	Point2f m_MousePos;
	Point2f m_LastCamMove;
	float m_Angle;

	bool m_LMouse = false;
	bool m_ShotFired = false;
	bool m_Meleeing = false;
	bool m_DecreaseAmmo = false;
	bool m_NoFire = false;

	std::vector<CasingSprite*> m_CasingSpriteArr;

	

	unsigned short m_RowOffset;
	unsigned short m_CurrentRow;

	Texture* m_pCrossHairTex;
	
	void DrawMouseLine() const;
	void CalcMouseAngle();
	void HandleRegen(float elapsedSec);
	void HandleAmmo(float elapsedSec);
	void InputStateAction();
	void UpdateFramesState();
	void UpdateCurrentFrame(float elapsedSec);
	void SpawnCasing(const StaticTextures& textures);
	void DeleteCasing();
	void DrawCasing() const;
	void UpdateCasing(float elapsedSec, const Level& level, const StaticTextures& textures);
	
	
};

