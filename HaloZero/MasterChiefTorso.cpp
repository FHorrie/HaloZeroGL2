#include "pch.h"
#include "debugpch.h"
#include "MasterChiefTorso.h"
#include "CasingSprite.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

MasterChiefTorso::MasterChiefTorso(const StaticTextures& textures, const SoundManager& sounds, const Point2f& startLocation, unsigned short framerate, const Rectf& window)
	:MasterChiefBase("MasterChiefTorso", textures, startLocation, framerate, 11, 5)
	, m_RegenAccuTime{}
	, m_MaxRegenTime{2.f}
	, m_ShieldAccuTime{}
	, m_MaxShieldTime{0.02f}

	, m_PrimaryAmmo{60}
	, m_PrimaryReserve{60}
	, m_PrimaryReloadTime{}
	, m_SecondaryAmmo{10}
	, m_SecondaryReserve{20}
	, m_SecondaryReloadTime{}

	, m_FirstSlot{ GunType::smart }
	, m_SecondSlot{ GunType::magnum }

	, m_pSmartRifleShot{ sounds.GetSoundEffect("SmartRifleShot") }
	, m_pMagnumShot{ sounds.GetSoundEffect("MagnumShot") }
	, m_pPlasmaPistolShot{ sounds.GetSoundEffect("PlasmaPistolShot") }
	, m_pPlasmaRifleShot{ sounds.GetSoundEffect("PlasmaRifleShot") }
	, m_pNeedlerShot{ sounds.GetSoundEffect("NeedlerShot") }
	, m_pMeleeSound{ sounds.GetSoundEffect("Melee")}

	, m_ShieldBeepTime{}

	, m_pShieldBeep{ sounds.GetSoundEffect("ShieldBeep") }
	, m_pShieldRecharge{ sounds.GetSoundEffect("ShieldRecharge")}

	, m_MousePos{ Point2f{2 * window.width / 3.f, window.height / 1.8f} }
	, m_LastCamMove{}
	, m_Angle{}
	, m_RowOffset{}
	, m_CurrentRow{}
	, m_CasingSpriteArr{}
	, m_pCrossHairTex{new Texture("MasterChief/CrossHairs.png")}
{
}

MasterChiefTorso::~MasterChiefTorso()
{
	delete m_pCrossHairTex;
}

void MasterChiefTorso::Draw() const
{
	Rectf srcRect{};
	srcRect.width = m_pSpriteTexture->GetWidth() / m_Cols;
	srcRect.height = m_pSpriteTexture->GetHeight() / m_Rows;
	srcRect.left = (m_CurrentFrame % m_Cols) * srcRect.width;
	srcRect.bottom = srcRect.height * (m_CurrentRow * 2 + m_RowOffset + 1);

	Rectf destRect{};
	destRect.left = - m_HitBox.width/2;
	destRect.bottom = 0;
	destRect.width = srcRect.width;
	destRect.height = srcRect.height;

	
	glPushMatrix();
	glTranslatef(m_HitBox.left + 4 * m_HitBox.width / 7, m_HitBox.bottom + m_HitBox.height - 50, 0);

	if (m_IsFlipped)
		glTranslatef(-m_HitBox.width / 8, 0, 0);

	glRotatef(m_Angle / float(M_PI) * 180.f, 0, 0, 1);

	if(m_IsFlipped)
		glScalef(-1, 1, 1);

	m_pSpriteTexture->Draw(destRect, srcRect);
	glPopMatrix();

	GunType activeGun{ (!m_IsSecondaryEquipped) ? m_FirstSlot : m_SecondSlot };

	m_pCrossHairTex->Draw(Rectf(m_MousePos.x - m_pCrossHairTex->GetWidth() / 10, m_MousePos.y - m_pCrossHairTex->GetHeight() / 2, m_pCrossHairTex->GetWidth()/5, m_pCrossHairTex->GetHeight())
		, Rectf((m_pCrossHairTex->GetWidth()/5) * int(activeGun), 0, m_pCrossHairTex->GetWidth() / 5, m_pCrossHairTex->GetHeight()));

	DrawCasing();

#ifdef _DEBUG_HITBOX
		utils::SetColor(Color4f(0.f, 1.f, 1.f, 1.f));
		DrawMouseLine();
#endif
	
}

void MasterChiefTorso::Update(float elapsedSec, const Level& level, const Rectf& hitbox, const StaticTextures& textures)
{
	m_HitBox = hitbox;
	CalcMouseAngle();
	UpdateFramesState();
	InputStateAction();
	UpdateCurrentFrame(elapsedSec);
	UpdateCasing(elapsedSec, level, textures);
	HandleRegen(elapsedSec);
	HandleAmmo(elapsedSec);
}

void MasterChiefTorso::SaveMousePos(const Point2f& mousePos)
{
	m_MousePos = mousePos;
}

void MasterChiefTorso::AdjustMousePos(const Point2f& movement)
{
	Point2f stablePos{ movement - m_LastCamMove };
	m_MousePos.x += stablePos.x;
	m_MousePos.y += stablePos.y;
	m_LastCamMove = movement;
}

void MasterChiefTorso::DrawMouseLine() const 
{
	Point2f startPoint{ m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 3 * m_HitBox.height / 4 };
	utils::DrawLine(startPoint, m_MousePos);
}

void MasterChiefTorso::CalcMouseAngle()
{
	Point2f startPoint = Point2f(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 3 * m_HitBox.height / 4);
	float pi{ float(M_PI) };
	float angle{ atan2f(m_MousePos.y - startPoint.y, m_MousePos.x - startPoint.x)};
	if (angle > pi/2 || angle < -pi/2)
		m_IsFlipped = true;
	else
		m_IsFlipped = false;

	if (m_IsFlipped)
	{
		angle += pi;
	}

	m_Angle = angle;
}

void MasterChiefTorso::DrawCrossHair() const
{
	
}

//void MasterChiefTorso::HandleShotHits(Rectf enemyShape)
//{
//	m_ShotHitBox = Rectf(m_HitBox.left + m_HitBox.width / 2, m_HitBox.bottom + 3 * m_HitBox.height / 4, (!m_IsFlipped) ? 100 : -100, m_MousePos.y);
//	if (utils::IsOverlapping(m_ShotHitBox, enemyShape))
//	{
//
//	}
//}

void MasterChiefTorso::HandleFire(bool activate)
{
	m_LMouse = activate;
	if (m_ActionState != ActionState::grenade && m_ActionState != ActionState::melee)
	{
		if (m_LMouse && m_ActionState != ActionState::shooting)
			ChangeActionState(ActionState::shooting);
	}
}

void MasterChiefTorso::HandleMelee()
{
	if (m_ActionState != ActionState::grenade && m_ActionState != ActionState::melee)
	{
		ChangeActionState(ActionState::melee);
		m_Meleeing = true;
	}
}

void MasterChiefTorso::TakeDamage(int damage)
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

	if(m_Health < 0)
	{
		//std::cout << "YOU'RE DEAD\n";
		m_Dead = true;
		m_Health = 0;
		m_Shield = 0;
		return;
	}
}

void MasterChiefTorso::HandleRegen(float elapsedSec)
{
	if (m_RegenAccuTime > 0.f)
	{
		m_RegenAccuTime -= elapsedSec;

		m_ShieldBeepTime += elapsedSec;
		if (m_ShieldBeepTime >= 0.2f)
		{
			m_pShieldBeep->Play(0);
			m_ShieldBeepTime -= 0.2f;
		}
		/*if (m_RechargePlayed)
		{
			m_pShieldRecharge->StopAll();
			m_RechargePlayed = false;
		}*/
	}
	else if (m_Shield < m_MaxShield)
	{	
		m_ShieldAccuTime += elapsedSec;
		if(m_ShieldAccuTime >= m_MaxShieldTime)
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
		/*if (m_RechargePlayed)
		{
			m_pShieldRecharge->StopAll();
			m_RechargePlayed = false;
		}*/
		m_RechargePlayed = false;
		m_ShieldBeepTime = 0.f;
		m_ShieldAccuTime = 0.f;
	}


}

void MasterChiefTorso::HandleAmmo(float elapsedSec)
{
	if (m_DecreaseAmmo)
	{
		(!m_IsSecondaryEquipped) ? --m_PrimaryAmmo : --m_SecondaryAmmo;
		//std::cout << "PA: " << m_PrimaryAmmo << " SA: " << m_SecondaryAmmo << "\n";
		m_DecreaseAmmo = false;
	}

	if (m_PrimaryAmmo <= 0 && !m_IsSecondaryEquipped)
	{
		m_NoFire = true;
		m_PrimaryReloadTime += elapsedSec;
		if (m_PrimaryReloadTime >= 2.f && m_PrimaryReserve > 0)
		{
			int magSize{};
			switch (m_FirstSlot)
			{
			case GunType::smart:
				magSize = 60;
				break;
			case GunType::magnum:
				magSize = 10;
				break;
			case GunType::pistol:

				break;
			case GunType::rifle:

				break;
			case GunType::needle:
				magSize = 20;
				break;
			}
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
			m_PrimaryReloadTime = 0;
		}
	}

	else if (m_SecondaryAmmo <= 0 && m_IsSecondaryEquipped)
	{
		m_NoFire = true;
		m_SecondaryReloadTime += elapsedSec;
		if (m_SecondaryReloadTime >= 2.f && m_SecondaryReserve > 0)
		{
			int magSize{};
			switch (m_SecondSlot)
			{
			case GunType::smart:
				magSize = 60;
				break;
			case GunType::magnum:
				magSize = 10;
				break;
			case GunType::pistol:

				break;
			case GunType::rifle:

				break;
			case GunType::needle:
				magSize = 20;
				break;
			}
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
			m_SecondaryReloadTime = 0;
		}
	}
	else m_NoFire = false;
}

void MasterChiefTorso::InputStateAction()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);

	//Action states
	if (pStates[SDL_SCANCODE_G])
	{
		//if (m_ActionState != ActionState::grenade)
			m_ActionState = ActionState::grenade;
	}
	if (pStates[SDL_SCANCODE_1])
		m_IsSecondaryEquipped = false;

	if (pStates[SDL_SCANCODE_2])
		m_IsSecondaryEquipped = true;
}

void MasterChiefTorso::UpdateFramesState()
{

	switch (m_ActionState)
	{
	case ActionState::holding:
		m_nFrames = 1;

		if (!m_IsSecondaryEquipped)
			m_CurrentRow = int(m_FirstSlot);
		else 
			m_CurrentRow = int(m_SecondSlot);

		m_RowOffset = 0;
		m_Looped = false;
		break;
	case ActionState::shooting:
		m_nFrames = 2;
		if (!m_IsSecondaryEquipped)
		{
			switch (m_FirstSlot)
			{
			case GunType::smart:
				m_Framerate = 15;
				break;
			case GunType::magnum:
				m_Framerate = 8;
				break;
			case GunType::pistol:
				m_Framerate = 10;
				break;
			case GunType::rifle:
				m_Framerate = 10;
				break;
			case GunType::needle:
				m_Framerate = 10;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (m_SecondSlot)
			{
			case GunType::smart:
				m_Framerate = 12;
				break;
			case GunType::magnum:
				m_Framerate = 8;
				break;
			case GunType::pistol:
				m_Framerate = 10;
				break;
			case GunType::rifle:
				m_Framerate = 10;
				break;
			case GunType::needle:
				m_Framerate = 10;
				break;
			default:
				break;
			}
		}
		
		if (!m_IsSecondaryEquipped)
			m_CurrentRow = int(m_FirstSlot);
		else 
			m_CurrentRow = int(m_SecondSlot);

		m_RowOffset = 0;
		m_Looped = true;
		break;
	case ActionState::melee:
		m_nFrames = 2;
		m_Framerate = 8;

		if (!m_IsSecondaryEquipped)
			m_CurrentRow = int(m_FirstSlot);
		else 
			m_CurrentRow = int(m_SecondSlot);

		m_RowOffset = 1;
		m_Looped = false;
		break;
	case ActionState::grenade:
		m_nFrames = 5;
		m_Framerate = 10;
		m_CurrentRow = 5;
		m_RowOffset = 0;
		m_Looped = false;
		break;
	}
}

void MasterChiefTorso::UpdateCurrentFrame(float elapsedSec)
{
	if (m_nFrames <= 1)
	{
		m_CurrentFrame = 0;
		return;
	}
	if (m_NoFire)
	{
		if (m_ActionState == ActionState::shooting)
			ChangeActionState(ActionState::holding);
	}

	m_AccuTime += elapsedSec;

	if (m_AccuTime >= (1.f / m_Framerate))
	{
		m_AccuTime -= 1.f / m_Framerate;

		if (m_CurrentFrame < (m_nFrames - 1))
		{
			m_CurrentFrame++;
		}
		else
		{
			if (m_ActionState == ActionState::shooting)
			{
				m_ShotFired = true;
				m_DecreaseAmmo = true;
				if(!m_LMouse || m_NoFire)
					ChangeActionState(ActionState::holding);
			}

			if (m_Looped)
				m_CurrentFrame = 0;

			else if (m_ActionState == ActionState::grenade || m_ActionState == ActionState::melee)
				ChangeActionState(ActionState::holding);

		}
		
	}
}

void MasterChiefTorso::SpawnCasing(const StaticTextures& textures)
{
	if (m_ShotFired && (GetActiveGun() == GunType::smart || GetActiveGun() == GunType::magnum))
	{
		Point2f startPoint{};
		if(m_IsFlipped)
			startPoint = Point2f(m_HitBox.left + m_HitBox.width / 4, m_HitBox.bottom + 3 * m_HitBox.height / 4 );
		else
			startPoint = Point2f(m_HitBox.left + 3 * (m_HitBox.width / 4), m_HitBox.bottom + 3 * m_HitBox.height / 4);


		m_CasingSpriteArr.push_back( new CasingSprite(textures, startPoint, cosf(m_Angle), sin(m_Angle), m_IsFlipped));
	}
}

void MasterChiefTorso::DeleteCasing()
{
	for (int i {}; i < m_CasingSpriteArr.size(); i++)
	{
		if (m_CasingSpriteArr[i]->CheckDeletion())
		{
			delete m_CasingSpriteArr[i];
			m_CasingSpriteArr.erase(m_CasingSpriteArr.begin() + i);
		}
	}
}

void MasterChiefTorso::DrawCasing() const
{
	for (int i{}; i < m_CasingSpriteArr.size(); i++)
	{
		m_CasingSpriteArr[i]->Draw();
	}
}

void MasterChiefTorso::UpdateCasing(float elapsedSec, const Level& level, const StaticTextures& textures)
{
	SpawnCasing(textures);
	for (int i {}; i < m_CasingSpriteArr.size(); i++)
	{
		m_CasingSpriteArr[i]->Update(elapsedSec, level);
	}
	DeleteCasing();
}

bool MasterChiefTorso::GetFlipped() const
{
	return m_IsFlipped;
}

bool MasterChiefTorso::IsShooting()
{
	bool tempShot{ m_ShotFired };
	m_ShotFired = false;

	if (tempShot)
	{
		if (!m_IsSecondaryEquipped)
		{
			switch (m_FirstSlot)
			{
			case GunType::smart:
				m_pSmartRifleShot->Play(0);
				break;
			case GunType::magnum:
				m_pMagnumShot->Play(0);
				break;
			case GunType::pistol:
				m_pPlasmaPistolShot->Play(0);
				break;
			case GunType::rifle:
				m_pPlasmaRifleShot->Play(0);
				break;
			case GunType::needle:
				m_pNeedlerShot->Play(0);
				break;
			}
		}
		else
		{
			switch (m_SecondSlot)
			{
			case GunType::smart:
				m_pSmartRifleShot->Play(0);
				break;
			case GunType::magnum:
				m_pMagnumShot->Play(0);
				break;
			case GunType::pistol:
				m_pPlasmaPistolShot->Play(0);
				break;
			case GunType::rifle:
				m_pPlasmaRifleShot->Play(0);
				break;
			case GunType::needle:
				m_pNeedlerShot->Play(0);
				break;
			}
		}

	}
	return tempShot;
}

bool MasterChiefTorso::IsMelee()
{
	bool tempMelee{ m_Meleeing };
	m_Meleeing = false;
	return tempMelee;
}

void MasterChiefTorso::PlayMeleeSound()
{
	m_pMeleeSound->Play(0);
}

void MasterChiefTorso::ChangeGun(GunType type, int ammo, int reserve)
{
	if (!m_IsSecondaryEquipped)
	{
		m_FirstSlot = type;
		m_PrimaryAmmo = ammo;
		m_PrimaryReserve = reserve;
	}
	else
	{
		m_SecondSlot = type;
		m_SecondaryAmmo = ammo;
		m_SecondaryReserve = reserve;
	}
}

void MasterChiefTorso::AddReserve(int reserve, GunType type)
{
	if (m_FirstSlot == type)
	{
		if (m_FirstSlot != GunType::pistol && m_FirstSlot != GunType::rifle)
		{
			m_PrimaryReserve += reserve;
			if (m_PrimaryReserve > 999)
				m_PrimaryReserve = 999;
		}
		else
		{
			m_PrimaryAmmo += reserve;
			if (m_PrimaryAmmo > 100)
				m_PrimaryAmmo = 100;
		}
	}
	else
	{
		if (m_SecondSlot != GunType::pistol && m_SecondSlot != GunType::rifle)
		{
			m_SecondaryReserve += reserve;
			if (m_SecondaryReserve > 999)
				m_SecondaryReserve = 999;
		}
		else
		{
			m_SecondaryAmmo += reserve;
			if (m_SecondaryAmmo > 100)
				m_SecondaryAmmo = 100;
		}
	}
}

GunType MasterChiefTorso::GetActiveGun() const
{
	return (!m_IsSecondaryEquipped) ? m_FirstSlot : m_SecondSlot;
}

GunType MasterChiefTorso::GetPrimary() const
{
	return m_FirstSlot;
}

GunType MasterChiefTorso::GetSecondary() const
{
	return m_SecondSlot;
}

int MasterChiefTorso::GetAmmo() const
{
	return (!m_IsSecondaryEquipped) ? m_PrimaryAmmo : m_SecondaryAmmo;
}

int MasterChiefTorso::GetReserve() const
{
	return (!m_IsSecondaryEquipped) ? m_PrimaryReserve : m_SecondaryReserve;
}

float MasterChiefTorso::GetAngle() const
{
	return m_Angle;
}

float MasterChiefTorso::GetShield(bool max) const
{
	return (max) ? m_MaxShield : m_Shield;
}

float MasterChiefTorso::GetHealth(bool max) const
{
	return (max) ? m_MaxHealth : m_Health;
}

bool MasterChiefTorso::IsDead() const
{
	return m_Dead;
}