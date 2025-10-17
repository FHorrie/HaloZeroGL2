#include "pch.h"
#include "Game.h"
#include "utils.h"
#include "Texture.h"

#include "MasterChiefLegs.h"
#include "MasterChiefTorso.h"
#include "EnvironmentSprite.h"
#include "GunPickup.h"
#include "Projectile.h"
#include "EnemyBase.h"
#include "ImpPistol.h"
#include "ImpNeedle.h"
#include "EliteRifle.h"
#include "Jackal.h"
#include "Hunter.h"
#include <iostream>
#include <fstream>

Game::Game(const Window& window)
	: BaseGame{ window }
	, m_ControlsScreen{"Controls", m_Textures}
	, m_EndScreen{"EndScreen", m_Textures}
	, m_DeathScreen{"DeathScreen", m_Textures}
	, m_PlayerPtr{ std::make_unique<MasterChiefBase>(m_Textures, m_Sounds, GetViewPort(), Point2f(360, 440)) } //360, 11000
	, m_pPlayerDeath{ std::make_unique<EnvironmentSprite>("MasterChiefDeath", m_Textures, Point2f{}, 2, 4, 1, 3, false) }
	, m_Level{}
	, m_Camera{ GetViewPort().width, GetViewPort().height }
	, m_HUD{ m_Textures, GetViewPort(), m_PlayerPtr->GetShield(true), m_PlayerPtr->GetHealth(true) }
{
	
	Initialize();
}

Game::~Game( )
{
	Cleanup();
}

void Game::Initialize( )
{
	SetCamera();

	//Initialize enemies
	LoadElements("Covenant/EnemySpawn.csv");
	 
	//Initialize pick ups
	LoadElements("Weapons/PickupSpawn.csv");

	//Start music
	PlayLevelBGM();

	//Disable the DAMN CURSOR
	SDL_ShowCursor(false);
}

void Game::Cleanup( )
{
	ClearEnemies();
	ClearGunDrops();
	ClearProjectiles();
}

void Game::ReInit()
{
	SoundStream::Stop();

	Cleanup();

	m_PlayerPtr->SetPosition(360, 440);
	m_PlayerPtr->Reset();

	//InitEnemies();
	LoadElements("Covenant/EnemySpawn.csv");

	//InitGunDrops();
	LoadElements("Weapons/PickupSpawn.csv");

	m_PickedUp = false;
	m_ButtonPressed = false;
	m_DisplayControls = false;
	m_GameEnd = false;
	m_GameDead = false;
	m_FadeOut = false;
	m_EndAlpha = 0.f;

	PlayLevelBGM();
}

void Game::Update( float elapsedSec )
{
	if (!m_GameEnd)
	{
		m_PlayerPtr->SetFlipped(m_PlayerPtr->GetFlipped());
		m_PlayerPtr->AdjustMousePos(m_Camera.CameraPos(m_PlayerPtr->GetShape()));

		if (!m_PlayerPtr->IsDead())
		{
			m_PlayerPtr->Update(elapsedSec, m_Level, m_Textures);
			UpdateHUD();
		}
		else
		{
			if (!m_GameDead)
			{
				m_pPlayerDeath->SetLocation(Point2f(m_PlayerPtr->GetShape().left, m_PlayerPtr->GetShape().bottom));
				m_pPlayerDeath->ResetAnim();
				m_GameDead = true;
			}
			else
			{
				m_pPlayerDeath->Update(elapsedSec);
				if (m_pPlayerDeath->AnimFinished())
				{
					m_Sounds.GetSoundStream("Level1")->Stop();
					PlayDeathBGM();
					m_GameEnd = true;
				}
			};
		}

		for (int i{}; i < m_pEnemyArr.size(); i++)
		{
			m_pEnemyArr[i]->Update(elapsedSec, m_Level, m_PlayerPtr->GetShape());
		}
		HandleInteractions(elapsedSec);
		HandlePickups(elapsedSec);
		HandleEnemyDrop();
	}
	else if(!m_FadeOut)
	{
		if (m_EndAlpha < 1.f)
			m_EndAlpha += elapsedSec;
		else
		{
			m_EndAlpha = 1.f;
			m_FadeOut = true;
		}
	}
	else
	{
		if (m_EndAlpha > 0.f)
			m_EndAlpha -= elapsedSec;
		else
			m_EndAlpha = 0.f;
	}

	if (m_Level.HasReachedEnd(m_PlayerPtr->GetShape()))
	{
		if (!m_GameEnd)
		{
			//std::cout << "Good job, Master Chief!\nPress Q to Quit.\n\n";
			m_Sounds.GetSoundStream("Level1")->Stop();
			PlayWinBGM();
				
		}
		m_GameEnd = true;
	}
	
}

void Game::Draw( ) const
{
	ClearBackground( );
	glPushMatrix();
	glTranslatef(-m_Camera.CameraPos(m_PlayerPtr->GetShape()).x/20, -m_Camera.CameraPos(m_PlayerPtr->GetShape()).y/20, 0);
	
	m_Level.DrawOutside();
	
	glPopMatrix();

	glPushMatrix();
	//glScalef(2, 2, 1);
	glTranslatef(-m_Camera.CameraPos(m_PlayerPtr->GetShape()).x, -m_Camera.CameraPos(m_PlayerPtr->GetShape()).y, 0);

	m_Level.DrawBackground();
	
	DrawEntities();

	m_Level.DrawForeground();

	//m_PlayerPtr->DrawCrossHair();

	glPopMatrix();

	DrawScreens();
}

void Game::DrawEntities() const
{
	for (int i{}; i < m_pEnemyArr.size(); i++)
		m_pEnemyArr[i]->Draw();

	if (!m_PlayerPtr->IsDead())
	{		
		m_PlayerPtr->Draw();
	}
	else
	{
		m_pPlayerDeath->Draw();
	}

	for (int i{}; i < m_pGunArr.size(); i++)
		m_pGunArr[i]->Draw();

	for (int i{}; i < m_pPlayerProjectileArr.size(); i++)
		m_pPlayerProjectileArr[i]->Draw();

	for (int i{}; i < m_pEnemyProjectileArr.size(); i++)
		m_pEnemyProjectileArr[i]->Draw();
}

void Game::DrawScreens() const
{
	m_HUD.Draw();

	if (m_DisplayControls)
	{
		m_ControlsScreen.Draw();
	}

	if (m_GameEnd)
	{
		if (m_FadeOut)
		{
			(!m_GameDead) ? m_EndScreen.Draw() : m_DeathScreen.Draw();
		}
		utils::SetColor(Color4f(0.f, 0.f, 0.f, m_EndAlpha));
		utils::FillRect(Rectf(0, 0, GetViewPort().width, GetViewPort().height));
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	if (e.keysym.sym == SDLK_i)
	{
		m_DisplayControls = true;
	}
	
	if (m_GameEnd)
	{
		if (e.keysym.sym == SDLK_q)
			exit(0);
		if (e.keysym.sym == SDLK_r)
			ReInit();
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	if (e.keysym.sym == SDLK_i)
	{
		m_DisplayControls = false;
		PrintControls();
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	m_PlayerPtr->SaveMousePos(Point2f( e.x + m_Camera.CameraPos(m_PlayerPtr->GetShape()).x, e.y + m_Camera.CameraPos(m_PlayerPtr->GetShape()).y));
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_PlayerPtr->HandleFire(true);
		break;
	case SDL_BUTTON_RIGHT:
		m_PlayerPtr->HandleMelee();
		break;
	}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_PlayerPtr->HandleFire(false);
		break;
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.1f, 0.1f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::PrintControls() const
{
	std::cout << m_GameInfoText;
}

void Game::SetCamera()
{
	m_Camera.SetLevelBoundaries(m_Level.GetBoundaries());
}

void Game::UpdateHUD()
{
	m_HUD.Update(m_PlayerPtr->GetShield(false), m_PlayerPtr->GetHealth(false), 
		m_PlayerPtr->GetActiveGun(), m_PlayerPtr->GetAmmo(), m_PlayerPtr->GetReserve());
}

void Game::PlayLevelBGM() const
{
	m_Sounds.GetSoundStream("Level1")->SetVolume(25);
	m_Sounds.GetSoundStream("Level1")->Play(true);
}

void Game::PlayWinBGM() const
{
	m_Sounds.GetSoundStream("GameWin")->SetVolume(70);
	m_Sounds.GetSoundStream("GameWin")->Play(true);
}

void Game::PlayDeathBGM() const
{
	m_Sounds.GetSoundStream("GameDead")->SetVolume(50);
	m_Sounds.GetSoundStream("GameDead")->Play(true);
}

void Game::LoadElements(std::string fileLocation)
{
	std::ifstream dataFile{ fileLocation };
	
	if (!dataFile)
	{
		std::cout << "Error occured while opening file\n";
		return;
	}

	std::string type{};
	std::getline(dataFile, type);
	if (type == "Enemies")
	{
		while (!dataFile.eof())
		{
			//std::cout << "Worked well\n";
			std::string enemyType, x, y;
			std::getline(dataFile, enemyType, ',');
			std::getline(dataFile, x, ',');
			std::getline(dataFile, y);
			EnemyBase* pEnemy{};
			if (enemyType == "ImpPistol")
			{
				pEnemy = new ImpPistol(m_Textures, m_Sounds, Point2f{ stof(x), stof(y) });
			}
			else if (enemyType == "ImpNeedle")
			{
				pEnemy = new ImpNeedle(m_Textures, m_Sounds, Point2f{ stof(x), stof(y) });
			}
			else if (enemyType == "Jackal")
			{
				pEnemy = new Jackal(m_Textures, m_Sounds, Point2f{ stof(x), stof(y) });
			}
			else if (enemyType == "EliteRifle")
			{
				pEnemy = new EliteRifle(m_Textures, m_Sounds, Point2f{ stof(x), stof(y) });
			}
			else if (enemyType == "Hunter")
			{
				pEnemy = new Hunter(m_Textures, m_Sounds, Point2f{ stof(x), stof(y) });
			}
			else
			{
				exit(421);
			}

			m_pEnemyArr.push_back(pEnemy);

		}
	}
	else if (type == "Guns")
	{
		while (!dataFile.eof())
		{
			std::string gunType, x, y;
			//std::cout << "Worked better\n";

			std::getline(dataFile, x, ',');
			std::getline(dataFile, y, ',');
			std::getline(dataFile, gunType);
			GunPickup* pPickup{};
			if (gunType == "smart")
			{
				pPickup = new GunPickup(m_Textures, Point2f{ stof(x), stof(y) }, GunType::SmartRifle);
			}
			else if (gunType == "magnum")
			{
				pPickup = new GunPickup(m_Textures, Point2f{ stof(x), stof(y) }, GunType::MagnumPistol);
			}
			else if (gunType == "pistol")
			{
				pPickup = new GunPickup(m_Textures, Point2f{ stof(x), stof(y) }, GunType::PlasmaPistol);
			}
			else if (gunType == "rifle")
			{
				pPickup = new GunPickup(m_Textures, Point2f{ stof(x), stof(y) }, GunType::PlasmaRifle);
			}
			else if (gunType == "needle")
			{
				pPickup = new GunPickup(m_Textures, Point2f{ stof(x), stof(y) }, GunType::Needler);
			}
			else
			{
				exit(422);
			}

			m_pGunArr.push_back(pPickup);
		}
	}
}

//void Game::InitEnemies()
//{
//	m_pEnemyArr.push_back(new ImpPistol(Point2f(1900, 110)));
//	m_pEnemyArr.push_back(new Hunter(Point2f(2300, 110)));
//	m_pEnemyArr.push_back(new ImpPistol(Point2f(2850, 140)));
//	m_pEnemyArr.push_back(new ImpPistol(Point2f(3650, 110)));
//	m_pEnemyArr.push_back(new Jackal(Point2f(4350, 440)));
//	m_pEnemyArr.push_back(new ImpNeedle(Point2f(4400, 440)));
//	m_pEnemyArr.push_back(new Jackal(Point2f(4600, 440)));
//	m_pEnemyArr.push_back(new EliteRifle(Point2f(4900, 440)));
//	m_pEnemyArr.push_back(new ImpNeedle(Point2f(7000, 440)));
//	m_pEnemyArr.push_back(new EliteRifle(Point2f(7250, 440)));
//	
//}

void Game::ClearEnemies()
{
	for (int i {}; i < m_pEnemyArr.size(); i++)
	{
		delete m_pEnemyArr[i];
	}
	m_pEnemyArr.clear();
}

//void Game::InitGunDrops()
//{
//	m_pGunArr.push_back(new GunPickup(Point2f(550, 490), GunType::rifle));
//	m_pGunArr.push_back(new GunPickup(Point2f(1660, 120), GunType::smart));
//	m_pGunArr.push_back(new GunPickup(Point2f(6920, 490), GunType::magnum));
//	m_pGunArr.push_back(new GunPickup(Point2f(8410, 120), GunType::magnum));
//
//}

void Game::ClearGunDrops()
{
	for (int i{}; i < m_pGunArr.size(); i++)
	{
		delete m_pGunArr[i];
	}
	m_pGunArr.clear();
}

void Game::ClearProjectiles()
{
	for (int i{}; i < m_pPlayerProjectileArr.size(); i++)
	{
		delete m_pPlayerProjectileArr[i];
	}
	m_pPlayerProjectileArr.clear();

	for (int i{}; i < m_pEnemyProjectileArr.size(); i++)
	{
		delete m_pEnemyProjectileArr[i];
	}
	m_pEnemyProjectileArr.clear();
}

void Game::HandlePickups(float elapsedSec)
{
	for (int i{}; i < m_pGunArr.size(); i++)
	{
		m_pGunArr[i]->Update(elapsedSec, m_Level);
		if (m_pGunArr[i]->CheckOverlap(m_PlayerPtr->GetShape()))
		{
			PickupInteract(m_pGunArr[i]->GetGunType(), m_pGunArr[i]->GetAmmo(), m_pGunArr[i]->GetReserve());
			if (m_PickedUp)
			{
				delete m_pGunArr[i];
				m_pGunArr.erase(m_pGunArr.begin() + i);
				m_PickedUp = false;
			}
		}
	}
}

void Game::PickupInteract(GunType type, int ammo, int reserve)
{
	if (type != m_PlayerPtr->GetPrimary() && type != m_PlayerPtr->GetSecondary())
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (pStates[SDL_SCANCODE_LSHIFT])
		{
			if (!m_ButtonPressed)
			{
				m_pGunArr.push_back(new GunPickup(m_Textures, m_PlayerPtr->GetDropPoint(), m_PlayerPtr->GetActiveGun(), m_PlayerPtr->GetAmmo(), m_PlayerPtr->GetReserve()));
				m_PlayerPtr->ChangeGun(type, ammo, reserve, m_PlayerPtr->IsSecondaryEquipped());
				m_PickedUp = true;
				m_ButtonPressed = true;
			}
		}
		else m_ButtonPressed = false;
	}
	else
	{
		m_PlayerPtr->AddReserve(ammo + reserve, type);
		m_PickedUp = true;
	}
}

void Game::HandleInteractions(float elapsedSec)
{
	if (m_PlayerPtr->IsShooting())
	{
		NewPlayerProjectile(m_PlayerPtr->GetAngle());
	}
	if (m_PlayerPtr->IsMelee())
	{
		for (int i{}; i < m_pEnemyArr.size(); i++)
		{
			if (utils::IsOverlapping(m_pEnemyArr[i]->GetShape(), m_PlayerPtr->GetShape()) && !m_pEnemyArr[i]->IsDead())
			{
				m_PlayerPtr->PlayMeleeSound();
				m_pEnemyArr[i]->TakeDamage(50);
			}
		}
	}
	for (int i{}; i < m_pEnemyArr.size(); i++)
	{
		if (m_pEnemyArr[i]->IsShooting())
		{
			Point2f dspcdPnt{ m_PlayerPtr->GetShape().left - m_pEnemyArr[i]->GetShape().left, 
							m_PlayerPtr->GetShape().bottom - m_pEnemyArr[i]->GetShape().bottom };
			float enemyAngle{ atan2f(dspcdPnt.y, dspcdPnt.x)};

			NewEnemyProjectile(enemyAngle, i);
		}
		if (m_pEnemyArr[i]->IsMelee())
		{
			if (utils::IsOverlapping(m_pEnemyArr[i]->GetShape(), m_PlayerPtr->GetShape()))
			{
				m_PlayerPtr->PlayMeleeSound();
				m_PlayerPtr->TakeDamage(60);
			}
		}
	}
	for (int i{}; i < m_pPlayerProjectileArr.size(); i++)
	{
		m_pPlayerProjectileArr[i]->UpdatePosition(elapsedSec, m_Level);

		for (int j{}; j < m_pEnemyArr.size(); j++)
		{
			if (m_pPlayerProjectileArr[i]->CheckHit(m_pEnemyArr[j]->GetShape()))
			{
				m_pEnemyArr[j]->TakeDamage(m_pPlayerProjectileArr[i]->GetDamage());
			}
		}
		if (m_pPlayerProjectileArr[i]->CheckDeletion() || !utils::IsOverlapping(m_pPlayerProjectileArr[i]->GetHitBox(), Rectf(m_Camera.CameraPos(m_PlayerPtr->GetShape()).x, m_Camera.CameraPos(m_PlayerPtr->GetShape()).y, GetViewPort().width, GetViewPort().height)))
		{
			delete m_pPlayerProjectileArr[i];
			//std::cout << "DeleteProjectile\n";
			m_pPlayerProjectileArr.erase(m_pPlayerProjectileArr.begin() + i);
			break;
		}
	}
	for (int i{}; i < m_pEnemyProjectileArr.size(); i++)
	{
		m_pEnemyProjectileArr[i]->UpdatePosition(elapsedSec, m_Level);

		if (m_pEnemyProjectileArr[i]->CheckHit(m_PlayerPtr->GetShape()))
		{
			m_PlayerPtr->TakeDamage(m_pEnemyProjectileArr[i]->GetDamage());
		}

		if (m_pEnemyProjectileArr[i]->CheckDeletion() || !utils::IsOverlapping(m_pEnemyProjectileArr[i]->GetHitBox(), Rectf(m_Camera.CameraPos(m_PlayerPtr->GetShape()).x, m_Camera.CameraPos(m_PlayerPtr->GetShape()).y, GetViewPort().width, GetViewPort().height)))
		{
			delete m_pEnemyProjectileArr[i];
			//std::cout << "DeleteProjectile\n";
			m_pEnemyProjectileArr.erase(m_pEnemyProjectileArr.begin() + i);
			break;
		}
	}

}

void Game::NewPlayerProjectile(float angle)
{
	m_pPlayerProjectileArr.push_back(new Projectile(m_Textures
		, Point2f(m_PlayerPtr->GetShape().left + m_PlayerPtr->GetShape().width / 2
			, m_PlayerPtr->GetShape().bottom + m_PlayerPtr->GetShape().height / 2)
		, angle, m_PlayerPtr->GetActiveGun(), m_PlayerPtr->GetFlipped()));
}

void Game::NewEnemyProjectile(float angle, int idx)
{
	m_pEnemyProjectileArr.push_back(new Projectile(m_Textures
		, Point2f(m_pEnemyArr[idx]->GetShape().left + m_pEnemyArr[idx]->GetShape().width / 2
			, m_pEnemyArr[idx]->GetShape().bottom + m_pEnemyArr[idx]->GetShape().height / 2)
		, angle, m_pEnemyArr[idx]->GetWeapon(), m_pEnemyArr[idx]->GetFlipped()));
}

void Game::HandleEnemyDrop()
{
	for (int i{}; i < m_pEnemyArr.size(); i++)
	{
		if (m_pEnemyArr[i]->DropWeapon())
		{
			//std::cout << "ok";
			switch (m_pEnemyArr[i]->GetWeapon())
			{
			case EnemyGunType::pistol:
				m_pGunArr.push_back(new GunPickup{ m_Textures, m_pEnemyArr[i]->GetDropPoint(), GunType::PlasmaPistol });
				break;
			case EnemyGunType::rifle:
				m_pGunArr.push_back(new GunPickup{ m_Textures, m_pEnemyArr[i]->GetDropPoint(), GunType::PlasmaRifle });
				break;
			case EnemyGunType::needle:
				m_pGunArr.push_back(new GunPickup{ m_Textures, m_pEnemyArr[i]->GetDropPoint(), GunType::Needler });
				break;
			default:
				break;
			}
		}
	}
}