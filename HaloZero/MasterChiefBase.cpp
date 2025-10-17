#include "pch.h"
#include "debugpch.h"
#include "MasterChiefBase.h"
#include "MasterChiefLegs.h"
#include "MasterChiefTorso.h"
#include "AnimatedSprite.h"
#include "CasingSprite.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

MasterChiefBase::MasterChiefBase(const StaticTextures& textures,
	const SoundManager& sounds, const Rectf& window, const Point2f& startLocation)
	: m_HitBox{ startLocation.x, startLocation.y, 70, 104 }

	, m_LegsPtr{ std::make_unique<MasterChiefLegs>(textures, startLocation, 12, this) }
	, m_TorsoPtr{ std::make_unique<MasterChiefTorso>(textures, startLocation, 12, this) }

	, m_pSmartRifleShot{ sounds.GetSoundEffect("SmartRifleShot") }
	, m_pMagnumShot{ sounds.GetSoundEffect("MagnumShot") }
	, m_pPlasmaPistolShot{ sounds.GetSoundEffect("PlasmaPistolShot") }
	, m_pPlasmaRifleShot{ sounds.GetSoundEffect("PlasmaRifleShot") }
	, m_pNeedlerShot{ sounds.GetSoundEffect("NeedlerShot") }
	, m_pMeleeSound{ sounds.GetSoundEffect("Melee") }
	, m_pReloadSound{ sounds.GetSoundEffect("ReloadDone") }

	, m_pShieldBeep{ sounds.GetSoundEffect("ShieldBeep") }
	, m_pShieldRecharge{ sounds.GetSoundEffect("ShieldRecharge") }

	, m_MousePos{ 2 * window.width / 3.f, window.height / 1.8f }
	, m_CrossHairTexturePtr{ std::make_unique<Texture>("MasterChief/CrossHairs.png") }

{
}

void MasterChiefBase::Draw() const
{
	m_TorsoPtr->Draw();
	m_LegsPtr->Draw();


	Rectf crosshairSrcRect
	{ 
		m_MousePos.x - m_CrossHairTexturePtr->GetWidth() / 10,
		m_MousePos.y - m_CrossHairTexturePtr->GetHeight() / 2,
		m_CrossHairTexturePtr->GetWidth() / 5,
		m_CrossHairTexturePtr->GetHeight() 
	};
	Rectf crosshairDestRect
	{ 
		(m_CrossHairTexturePtr->GetWidth() / 5) * int(GetActiveGun()),
		0,
		m_CrossHairTexturePtr->GetWidth() / 5,
		m_CrossHairTexturePtr->GetHeight()
	};

	m_CrossHairTexturePtr->Draw(crosshairSrcRect, crosshairDestRect);

	DrawCasings();
}

void MasterChiefBase::Update(float elapsedSec, const Level& level, const StaticTextures& textures)
{
	UpdatePosition(elapsedSec, level);
	UpdateInput();
	
	if (m_NoFire)
	{
		if (m_ActionState == ActionState::Shooting)
			ChangeActionState(ActionState::Holding);
	}

	CalcMouseAngle();
	UpdateCasing(elapsedSec, level, textures);
	HandleRegen(elapsedSec);
	HandleAmmo(elapsedSec);

	m_TorsoPtr->Update(elapsedSec);
	m_LegsPtr->Update(elapsedSec);
}

void MasterChiefBase::Reset()
{
	m_IsFlipped = false;
	m_IsBackwards = false;
	m_Dead = false;

	m_Shield = m_MaxShield;
	m_Health = m_MaxHealth;

	ChangeGun(GunType::SmartRifle, 60, 60, false);
	ChangeGun(GunType::MagnumPistol, 10, 20, true);
	m_IsSecondaryEquipped = false;

	m_LMouse = false;
	m_ShotFired = false;
	m_Meleeing = false;
	m_DecreaseAmmo = false;
	m_NoFire = false;
	m_NoMovementUpdate = false;

}

void MasterChiefBase::SetPosition(Point2f pos)
{
	SetPosition(pos.x, pos.y);
}

void MasterChiefBase::SetPosition(float x, float y)
{
	m_HitBox.left = x;
	m_HitBox.bottom = y;
}

void MasterChiefBase::ChangeMovementState(MoveState state)
{
	m_MoveState = state;
	m_LegsPtr->Reset();
	
	if (m_MoveState == MoveState::WaitingCrouch || m_MoveState == MoveState::RunningCrouch)
	{
		m_HitBox.height = 75;
	}
	else
	{
		m_HitBox.height = 104;
	}
}

void MasterChiefBase::ChangeActionState(ActionState state)
{
	m_ActionState = state;
	m_TorsoPtr->Reset();
}

void MasterChiefBase::UpdatePosition(float elapsedSec, const Level& level)
{
	if (level.IsOnGround(m_HitBox, m_Velocity) == false)
	{
		m_Velocity.y += m_Acceleration.y * elapsedSec;
		m_JumpBuffer += elapsedSec;
		if (m_MoveState != MoveState::Jumping && m_JumpBuffer > 0.15f)
		{
			ChangeMovementState(MoveState::Jumping);
			m_NoMovementUpdate = true;
		}
	}
	else
	{
		if (m_MoveState != MoveState::Waiting && m_JumpBuffer > 0.f)
		{
			m_JumpBuffer = 0.f;
			ChangeMovementState(MoveState::Waiting);
			m_NoMovementUpdate = false;
		}

		switch (m_MoveState)
		{
		case MoveState::Waiting:
			m_Velocity = Vector2f{ 0.f, 0.f };
			break;
		case MoveState::Running:
			m_Velocity.x = m_MoveSpeed;
			break;
		case MoveState::Jumping:
			m_Velocity.y = m_JumpSpeed;
			break;
		case MoveState::WaitingCrouch:
			m_Velocity = Vector2f{ 0.f, 0.f };
			break;
		case MoveState::RunningCrouch:
			m_Velocity.x = m_CrouchMoveSpeed;
			break;
		default:
			break;
		}
	}

	m_IsBackwards == m_IsFlipped ? m_HitBox.left += m_Velocity.x *  elapsedSec : m_HitBox.left -= m_Velocity.x * elapsedSec;
	
	m_HitBox.bottom += m_Velocity.y * elapsedSec;

	level.HandleCollision(m_HitBox, m_Velocity);
}

void MasterChiefBase::UpdateInput()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	if (m_NoMovementUpdate) return;

	// Movement states
	if (pStates[SDL_SCANCODE_SPACE])
	{
		if (m_MoveState != MoveState::Jumping 
			&& m_MoveState != MoveState::WaitingCrouch 
			&& m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::Jumping);
	}
	else if (pStates[SDL_SCANCODE_D] && pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::RunningCrouch);
		m_IsBackwards = m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_A] && pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::RunningCrouch)
			ChangeMovementState(MoveState::RunningCrouch);
		m_IsBackwards = !m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_S])
	{
		if (m_MoveState != MoveState::WaitingCrouch)
			ChangeMovementState(MoveState::WaitingCrouch);
	}
	else if (pStates[SDL_SCANCODE_D])
	{
		if (m_MoveState != MoveState::Running)
			ChangeMovementState(MoveState::Running);
		m_IsBackwards = m_IsFlipped;
	}
	else if (pStates[SDL_SCANCODE_A])
	{
		if (m_MoveState != MoveState::Running)
			ChangeMovementState(MoveState::Running);
		m_IsBackwards = !m_IsFlipped;
	}
	else
	{
		if (m_MoveState != MoveState::Waiting)
			ChangeMovementState(MoveState::Waiting);
	}

	// Action states
	if (pStates[SDL_SCANCODE_G])
	{
		m_ActionState = ActionState::Grenade;
	}

	if (pStates[SDL_SCANCODE_1])
	{
		m_IsSecondaryEquipped = false;
	}
	else if (pStates[SDL_SCANCODE_2])
	{
		m_IsSecondaryEquipped = true;
	}
}

void MasterChiefBase::SaveMousePos(const Point2f& mousePos)
{
	m_MousePos = mousePos;
}

void MasterChiefBase::AdjustMousePos(const Point2f& movement)
{
	Point2f stablePos{ movement - m_LastCamMove };
	m_MousePos.x += stablePos.x;
	m_MousePos.y += stablePos.y;
	m_LastCamMove = movement;
}

bool MasterChiefBase::IsShooting()
{
	bool tempShot{ m_ShotFired };
	m_ShotFired = false;

	if (tempShot)
	{
		switch (m_IsSecondaryEquipped ? m_SecondSlot : m_FirstSlot)
		{
		case GunType::SmartRifle:
			m_pSmartRifleShot->Play(0);
			break;
		case GunType::MagnumPistol:
			m_pMagnumShot->Play(0);
			break;
		case GunType::PlasmaPistol:
			m_pPlasmaPistolShot->Play(0);
			break;
		case GunType::PlasmaRifle:
			m_pPlasmaRifleShot->Play(0);
			break;
		case GunType::Needler:
			m_pNeedlerShot->Play(0);
			break;
		}
	}
	return tempShot;
}

bool MasterChiefBase::IsMelee()
{
	bool tempMelee{ m_Meleeing };
	m_Meleeing = false;
	return tempMelee;
}

void MasterChiefBase::PlayMeleeSound()
{
	m_pMeleeSound->Play(0);
}

void MasterChiefBase::ChangeGun(GunType type, int ammo, int reserve, bool isSecondary)
{
	if (isSecondary)
	{
		m_SecondSlot = type;
		m_SecondaryAmmo = ammo;
		m_SecondaryReserve = reserve;
	}
	else
	{
		m_FirstSlot = type;
		m_PrimaryAmmo = ammo;
		m_PrimaryReserve = reserve;
	}
}

void MasterChiefBase::AddReserve(int reserve, GunType type)
{
	bool isPrimary{ m_FirstSlot == type };
	int& selectedReserve{ isPrimary ? m_PrimaryReserve : m_SecondaryReserve };
	int& selectedAmmo{ isPrimary ? m_PrimaryAmmo : m_SecondaryAmmo };

	if (type != GunType::PlasmaPistol && type != GunType::PlasmaRifle)
	{
		selectedReserve += reserve;
		if (selectedReserve > 999)
			selectedReserve = 999;
	}
	else
	{
		selectedAmmo += reserve;
		if (selectedAmmo > 100)
			selectedAmmo = 100;
	}
}

void MasterChiefBase::TorsoAnimationLoopCompleted()
{
	if (m_ActionState == ActionState::Shooting)
	{
		m_ShotFired = true;
		m_DecreaseAmmo = true;
		if (m_LMouse == false || m_NoFire)
			ChangeActionState(ActionState::Holding);
	}
	else if (m_ActionState == ActionState::Grenade || m_ActionState == ActionState::Melee)
	{
		ChangeActionState(ActionState::Holding);
	}
}

void MasterChiefBase::DrawMouseLine() const
{
	Point2f startPoint{ m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 3 * m_HitBox.height / 4 };
	utils::DrawLine(startPoint, m_MousePos);
}

void MasterChiefBase::CalcMouseAngle()
{
	Point2f startPoint = Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 3 * m_HitBox.height / 4);
	float pi{ float(M_PI) };
	float angle{ atan2f(m_MousePos.y - startPoint.y, m_MousePos.x - startPoint.x) };

	m_IsFlipped = angle > pi / 2 || angle < -pi / 2;

	if (m_IsFlipped)
	{
		angle += pi;
	}

	m_Angle = angle;
}

void MasterChiefBase::HandleFire(bool activate)
{
	m_LMouse = activate;
	if (m_ActionState != ActionState::Grenade && m_ActionState != ActionState::Melee)
	{
		if (m_LMouse && m_ActionState != ActionState::Shooting)
			ChangeActionState(ActionState::Shooting);
	}
}

void MasterChiefBase::HandleMelee()
{
	if (m_ActionState != ActionState::Grenade && m_ActionState != ActionState::Melee)
	{
		ChangeActionState(ActionState::Melee);
		m_Meleeing = true;
	}
}

void MasterChiefBase::TakeDamage(int damage)
{
	m_RegenAccuTime = m_MaxRegenTime;
	if (m_Shield >= 0 && !m_Dead)
	{
		m_Shield -= damage;
		//std::cout << "Shield: " << m_Shield << "\n";
	}
	else if (m_Health >= 0 && !m_Dead)
	{
		m_Health -= damage;
		//std::cout << "Health: " << m_Health << "\n";
	}
	else;

	if (m_Health < 0)
	{
		//std::cout << "YOU'RE DEAD\n";
		m_Dead = true;
		m_Health = 0;
		m_Shield = 0;
		return;
	}
}

void MasterChiefBase::HandleRegen(float elapsedSec)
{
	if (m_RegenAccuTime > 0.f)
	{
		m_RegenAccuTime -= elapsedSec;

		m_ShieldBeepTime += elapsedSec;
		if (m_ShieldBeepTime >= 0.2f)
		{
			m_pShieldBeep->Play(0);
			m_ShieldBeepTime = 0.f;
		}
	}
	else if (m_Shield < m_MaxShield)
	{
		m_ShieldAccuTime += elapsedSec;
		if (m_ShieldAccuTime >= m_MaxShieldTime)
		{
			if (!m_RechargePlayed)
			{
				m_pShieldRecharge->Play(0);
				m_RechargePlayed = true;
			}
			m_Shield++;
			//std::cout << "Shield: " << m_Shield << "\n";
			m_ShieldAccuTime -= m_MaxShieldTime;
		}
	}
	else
	{
		m_RechargePlayed = false;
		m_ShieldBeepTime = 0.f;
		m_ShieldAccuTime = 0.f;
	}


}

void MasterChiefBase::HandleAmmo(float elapsedSec)
{
	if (m_DecreaseAmmo)
	{
		m_IsSecondaryEquipped == false ? --m_PrimaryAmmo : --m_SecondaryAmmo;
		//std::cout << "PA: " << m_PrimaryAmmo << " SA: " << m_SecondaryAmmo << "\n";
		m_DecreaseAmmo = false;
	}

	if (m_IsSecondaryEquipped == false && m_PrimaryAmmo <= 0)
	{
		m_NoFire = true;
		m_PrimaryReloadTime += elapsedSec;
		if (m_PrimaryReloadTime >= 2.f && m_PrimaryReserve > 0)
		{
			int magSize{ GetWeaponMagSize(m_FirstSlot) };
			if (m_PrimaryReserve <= magSize)
			{
				m_PrimaryAmmo = m_PrimaryReserve;
				m_PrimaryReserve = 0;
				//std::cout << "OUT OF AMMO, Primary reserve ammo: " << m_PrimaryReserve << "\n";
			}
			else
			{
				m_PrimaryAmmo = magSize;
				m_PrimaryReserve -= magSize;
				//std::cout << "RELOADING, Primary reserve ammo: " << m_PrimaryReserve << "\n";
			}
			m_pReloadSound->Play(0);
			m_PrimaryReloadTime = 0;
		}
	}

	else if (m_IsSecondaryEquipped && m_SecondaryAmmo <= 0)
	{
		m_NoFire = true;
		m_SecondaryReloadTime += elapsedSec;
		if (m_SecondaryReloadTime >= 2.f && m_SecondaryReserve > 0)
		{
			int magSize{ GetWeaponMagSize(m_SecondSlot) };
			if (m_SecondaryReserve <= magSize)
			{
				m_SecondaryAmmo = m_SecondaryReserve;
				m_SecondaryReserve = 0;
				//std::cout << "OUT OF AMMO, Secondary reserve Ammo: " << m_SecondaryReserve << "\n";
			}
			else
			{
				m_SecondaryAmmo = magSize;
				m_SecondaryReserve -= magSize;
				//std::cout << "RELOADING, Secondary Reserve Ammo: " << m_SecondaryReserve << "\n";
			}
			m_pReloadSound->Play(0);
			m_SecondaryReloadTime = 0;
		}
	}
	else m_NoFire = false;
}

int MasterChiefBase::GetWeaponMagSize(GunType type)
{
	switch (type)
	{
	case GunType::SmartRifle:
		return 60;
	case GunType::MagnumPistol:
		return 10;
	case GunType::Needler:
		return 20;
	default:
		return 0;
	}
}

void MasterChiefBase::SpawnCasing(const StaticTextures& textures)
{
	GunType currentGun = GetActiveGun();
	if (m_ShotFired && (currentGun == GunType::SmartRifle || currentGun == GunType::MagnumPistol))
	{
		Point2f startPoint{};
		startPoint = m_IsFlipped ? Point2f(m_HitBox.left + m_HitBox.width / 4, m_HitBox.bottom + 3 * m_HitBox.height / 4) 
								: Point2f(m_HitBox.left + 3 * (m_HitBox.width / 4), m_HitBox.bottom + 3 * m_HitBox.height / 4);

		m_CasingSpriteArr.push_back(std::make_unique<CasingSprite>(textures, startPoint, cosf(m_Angle), sin(m_Angle), m_IsFlipped));
	}
}

void MasterChiefBase::DeleteCasing()
{
	for (int i{}; i < m_CasingSpriteArr.size(); i++)
	{
		if (m_CasingSpriteArr[i]->CheckDeletion())
		{
			m_CasingSpriteArr.erase(m_CasingSpriteArr.begin() + i);
		}
	}
}

void MasterChiefBase::DrawCasings() const
{
	for (int i{}; i < m_CasingSpriteArr.size(); i++)
	{
		m_CasingSpriteArr[i]->Draw();
	}
}

void MasterChiefBase::UpdateCasing(float elapsedSec, const Level& level, const StaticTextures& textures)
{
	SpawnCasing(textures);
	for (int i{}; i < m_CasingSpriteArr.size(); i++)
	{
		m_CasingSpriteArr[i]->Update(elapsedSec, level);
	}
	DeleteCasing();
}