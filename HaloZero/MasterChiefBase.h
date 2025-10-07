#pragma once
#include "AnimatedSprite.h"
#include "Level.h"

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

class MasterChiefBase : public AnimatedSprite
{
public:
	MasterChiefBase(const std::string& spriteType, const StaticTextures& textures, const Point2f& startLocation, unsigned short framerate, unsigned short rows, unsigned short cols);
	virtual ~MasterChiefBase() = default;

	virtual void Draw() const override = 0;
	//virtual void Update(float elapsedSec, const Level& level) = 0;

	Rectf GetShape() const;
	Point2f GetDropPoint() const;

protected:
	Vector2f m_Acceleration;
	Vector2f m_Velocity;

	Rectf m_HitBox;

	MoveState m_MoveState{ MoveState::Waiting };
	ActionState m_ActionState{ ActionState::Holding };

	float m_JumpBuffer{};
	const float m_HorSpeed;
	const float m_JumpSpeed;

	bool m_IsFlipped{ false };
	bool m_IsBackwards{ false };
	bool m_IsDead{ false };
	
	void ChangeMovementState(MoveState state);
	void ChangeActionState(ActionState state);
	void UpdatePosition(float elapsedSec, const Level& level);

};

