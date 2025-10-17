#pragma once
#include "Level.h"
#include "SoundManager.h"
#include "Texture.h"
#include "CasingSprite.h"

enum class MoveState
{
	Waiting,
	Running,
	Jumping,
	WaitingCrouch,
	RunningCrouch
};

enum class ActionState
{
	Holding,
	Shooting,
	Melee,
	Grenade
};

enum class GunType
{
	SmartRifle,
	MagnumPistol,
	PlasmaPistol,
	PlasmaRifle,
	Needler
};

class Texture;
class CasingSprite;
class Projectile;
class AnimatedSprite;
class MasterChiefLegs;
class MasterChiefTorso;

class MasterChiefBase final
{
public:
	MasterChiefBase(const StaticTextures& textures, 
		const SoundManager& sounds, const Rectf& window, const Point2f& startLocation);
	virtual ~MasterChiefBase() = default;

	void Draw() const;
	void Update(float elapsedSec, const Level& level, const StaticTextures& textures);

	Rectf GetShape() const { return m_HitBox; }
	Point2f GetDropPoint() const 
	{ 
		return Point2f(m_HitBox.left + m_HitBox.width / 2,
			m_HitBox.bottom + 2 * m_HitBox.height / 3);
	}

	void SetPosition(Point2f pos);
	void SetPosition(float x, float y);

	void SaveMousePos(const Point2f& mousePos);
	void AdjustMousePos(const Point2f& movement);
	void SetFlipped(bool flipped) { m_IsFlipped = flipped; }

	void HandleFire(bool activate);
	void HandleMelee();
	void TakeDamage(int damage);

	bool IsShooting();
	bool IsMelee();
	void PlayMeleeSound();

	void ChangeGun(GunType type, int ammo, int reserve, bool isSecondary);
	void AddReserve(int reserve, GunType type);

	void TorsoAnimationLoopCompleted();

#pragma region Getters

	bool GetFlipped() const { return m_IsFlipped; }
	bool IsBackwards() const { return m_IsBackwards; }
	bool IsDead() const { return m_Dead; }
	float GetAngle() const { return m_Angle; }
	Rectf GetHitbox() const { return m_HitBox; }

	MoveState GetMoveState() const { return m_MoveState; }
	ActionState GetActionState() const { return m_ActionState; }

	GunType GetActiveGun() const { return m_IsSecondaryEquipped ? m_SecondSlot : m_FirstSlot; }
	GunType GetPrimary() const { return m_FirstSlot; }
	GunType GetSecondary() const { return m_SecondSlot; }
	bool IsSecondaryEquipped() const { return m_IsSecondaryEquipped; }

	int GetAmmo() const { return m_IsSecondaryEquipped ? m_SecondaryAmmo : m_PrimaryAmmo; }
	int GetReserve() const { return m_IsSecondaryEquipped ? m_SecondaryReserve : m_PrimaryReserve; }

	float GetShield(bool max) const { return max ? m_MaxShield : m_Shield; }
	float GetHealth(bool max) const { return max ? m_MaxHealth : m_Health; }

#pragma endregion

protected:
	const std::unique_ptr<MasterChiefLegs> m_LegsPtr{};
	const std::unique_ptr<MasterChiefTorso> m_TorsoPtr{};

	Vector2f m_Acceleration{ 0, -1200 };
	Vector2f m_Velocity{ 0, 0 };

	Rectf m_HitBox;

	MoveState m_MoveState{ MoveState::Waiting };
	ActionState m_ActionState{ ActionState::Holding };

	float m_JumpBuffer{};
	const float m_MoveSpeed{ 300 };
	const float m_CrouchMoveSpeed{ 200 };
	const float m_JumpSpeed{ 600 };

	bool m_IsFlipped{ false };
	bool m_IsBackwards{ false };
	bool m_IsDead{ false };

	const float m_MaxShield{ 150 };
	float m_Shield{ m_MaxShield };
	const float m_MaxHealth{ 70 };
	float m_Health{ m_MaxHealth };
	bool m_Dead{ false };

	float m_RegenAccuTime{};
	const float m_MaxRegenTime{ 2.f };
	float m_ShieldAccuTime{};
	const float m_MaxShieldTime{ 0.02f };

	int m_PrimaryAmmo{ 60 };
	int m_PrimaryReserve{ 60 };
	float m_PrimaryReloadTime{};

	int m_SecondaryAmmo{ 10 };
	int m_SecondaryReserve{ 20 };
	float m_SecondaryReloadTime{};

	const std::unique_ptr<Texture> m_CrossHairTexturePtr{};
	std::vector<std::unique_ptr<CasingSprite>> m_CasingSpriteArr{};
	GunType m_FirstSlot{ GunType::SmartRifle };
	GunType m_SecondSlot{ GunType::MagnumPistol };
	bool m_IsSecondaryEquipped{ false };

	const SoundEffect* m_pSmartRifleShot{};
	const SoundEffect* m_pMagnumShot{};
	const SoundEffect* m_pPlasmaPistolShot{};
	const SoundEffect* m_pPlasmaRifleShot{};
	const SoundEffect* m_pNeedlerShot{};
	const SoundEffect* m_pMeleeSound{};
	const SoundEffect* m_pReloadSound{};

	const SoundEffect* m_pShieldBeep;
	const SoundEffect* m_pShieldRecharge;


	float m_ShieldBeepTime{};
	bool m_RechargePlayed{ false };

	Point2f m_MousePos{};
	Point2f m_LastCamMove{};
	float m_Angle{};

	bool m_LMouse{ false };
	bool m_ShotFired{ false };
	bool m_Meleeing{ false };
	bool m_DecreaseAmmo{ false };
	bool m_NoFire{ false };
	bool m_NoMovementUpdate{ false };
	
	void ChangeMovementState(MoveState state);
	void ChangeActionState(ActionState state);
	void UpdatePosition(float elapsedSec, const Level& level);
	void UpdateInput();

	void DrawMouseLine() const;
	void CalcMouseAngle();
	void HandleRegen(float elapsedSec);
	void HandleAmmo(float elapsedSec);
	void SpawnCasing(const StaticTextures& textures);
	void DeleteCasing();
	void DrawCasings() const;
	void UpdateCasing(float elapsedSec, const Level& level, const StaticTextures& textures);
	int GetWeaponMagSize(GunType type);
};

