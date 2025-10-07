#pragma once
#include "AnimatedSprite.h"
#include "Level.h"

enum class MoveState
{
	waiting,
	running,
	jumping,
	waitingCrouch,
	runningCrouch
};

enum class ActionState
{
	holding,
	shooting,
	melee,
	grenade
};

enum class GunType
{
	smart,
	magnum,
	pistol,
	rifle,
	needle
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
	MoveState m_MoveState;
	ActionState m_ActionState;

	float m_JumpBuffer;

	Rectf m_HitBox;

	Vector2f m_DirVelocity;
	Vector2f m_Acceleration;
	const float m_HorSpeed;
	const float m_JumpSpeed;

	bool m_IsFlipped = false;
	bool m_IsBackwards = false;
	bool m_IsDead = false;

	
	void ChangeMovementState(MoveState state);
	void ChangeActionState(ActionState state);
	void UpdatePosition(float elapsedSec, const Level& level);

};

