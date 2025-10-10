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

	GunType GetActiveGun() const { return m_IsSecondaryEquipped ? m_SecondSlot : m_FirstSlot; }
	GunType GetPrimary() const { return m_FirstSlot; }
	GunType GetSecondary() const { return m_SecondSlot; }

	float GetAngle() const { return m_Angle; }

	int GetAmmo() const { return m_IsSecondaryEquipped ? m_SecondaryAmmo : m_PrimaryAmmo; }
	int GetReserve() const { return m_IsSecondaryEquipped ? m_SecondaryReserve : m_PrimaryReserve; }

	float GetShield(bool max) const { return max ? m_MaxShield : m_Shield; }
	float GetHealth(bool max) const { return max ? m_MaxHealth : m_Health; }
	bool IsDead() const { return m_Dead; }

private:

	const float m_MaxShield{ 150 };
	float m_Shield{ m_MaxShield };
	const float m_MaxHealth{ 70 };
	float m_Health{ m_MaxHealth };
	bool m_Dead{ false };

	float m_RegenAccuTime{};
	const float m_MaxRegenTime{2.f};
	float m_ShieldAccuTime{};
	const float m_MaxShieldTime{0.02f};

	int m_PrimaryAmmo{60};
	int m_PrimaryReserve{60};
	float m_PrimaryReloadTime{};

	int m_SecondaryAmmo{10};
	int m_SecondaryReserve{20};
	float m_SecondaryReloadTime{};

	Texture* m_pCrossHairTex{};
	GunType m_FirstSlot{ GunType::SmartRifle };
	GunType m_SecondSlot{ GunType::MagnumPistol };
	bool m_IsSecondaryEquipped = false;

	const SoundEffect* m_pSmartRifleShot{};
	const SoundEffect* m_pMagnumShot{};
	const SoundEffect* m_pPlasmaPistolShot{};
	const SoundEffect* m_pPlasmaRifleShot{};
	const SoundEffect* m_pNeedlerShot{};
	const SoundEffect* m_pMeleeSound{};
	const SoundEffect* m_pReloadSound{};

	float m_ShieldBeepTime{};
	bool m_RechargePlayed{ false };

	const SoundEffect* m_pShieldBeep;
	const SoundEffect* m_pShieldRecharge;

	Point2f m_MousePos{};
	Point2f m_LastCamMove{};
	float m_Angle{};

	bool m_LMouse{ false };
	bool m_ShotFired{ false };
	bool m_Meleeing{ false };
	bool m_DecreaseAmmo{ false };
	bool m_NoFire{ false };

	std::vector<CasingSprite*> m_CasingSpriteArr;

	unsigned short m_RowOffset{};
	unsigned short m_CurrentRow{};
	
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
	int GetWeaponMagSize(GunType type);
	
	
};

