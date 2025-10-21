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

		for (int i{}; i < m_EnemyPtrs.size(); i++)
		{
			m_EnemyPtrs[i]->Update(elapsedSec, m_Level, m_PlayerPtr->GetShape());
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
	for (int i{}; i < m_EnemyPtrs.size(); i++)
		m_EnemyPtrs[i]->Draw();

	if (!m_PlayerPtr->IsDead())
	{		
		m_PlayerPtr->Draw();
	}
	else
	{
		m_pPlayerDeath->Draw();
	}

	for (int i{}; i < m_GunPtrs.size(); i++)
		m_GunPtrs[i]->Draw();

	for (int i{}; i < m_PlayerProjectilePtrs.size(); i++)
		m_PlayerProjectilePtrs[i]->Draw();

	for (int i{}; i < m_EnemyProjectilePtrs.size(); i++)
		m_EnemyProjectilePtrs[i]->Draw();
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
	SoundStream const* const levelBgmPtr{ m_Sounds.GetSoundStream("Level1") };
	levelBgmPtr->SetVolume(25);
	levelBgmPtr->Play(true);
}

void Game::PlayWinBGM() const
{
	SoundStream const* const gameWinPtr{ m_Sounds.GetSoundStream("GameWin") };
	gameWinPtr->SetVolume(70);
	gameWinPtr->Play(true);
}

void Game::PlayDeathBGM() const
{
	SoundStream const* const deathBgmPtr{ m_Sounds.GetSoundStream("GameDead") };
	deathBgmPtr->SetVolume(50);
	deathBgmPtr->Play(true);
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

			if (enemyType == "ImpPistol")
			{
				m_EnemyPtrs.push_back(std::make_unique<ImpPistol>(m_Textures, m_Sounds, Point2f(stof(x), stof(y))));
			}
			else if (enemyType == "ImpNeedle")
			{
				m_EnemyPtrs.push_back(std::make_unique<ImpNeedle>(m_Textures, m_Sounds, Point2f(stof(x), stof(y))));
			}
			else if (enemyType == "Jackal")
			{
				m_EnemyPtrs.push_back(std::make_unique<Jackal>(m_Textures, m_Sounds, Point2f(stof(x), stof(y))));
			}
			else if (enemyType == "EliteRifle")
			{
				m_EnemyPtrs.push_back(std::make_unique<EliteRifle>(m_Textures, m_Sounds, Point2f(stof(x), stof(y))));
			}
			else if (enemyType == "Hunter")
			{
				m_EnemyPtrs.push_back(std::make_unique<Hunter>(m_Textures, m_Sounds, Point2f(stof(x), stof(y))));
			}
			else
			{
				exit(421);
			}
		}
	}
	else if (type == "Guns")
	{
		while (!dataFile.eof())
		{
			std::string gunText{}, x{}, y{};
			GunType gunType{};
			//std::cout << "Worked better\n";

			std::getline(dataFile, x, ',');
			std::getline(dataFile, y, ',');
			std::getline(dataFile, gunText);
			if (gunText == "smart")
			{
				gunType = GunType::SmartRifle;
			}
			else if (gunText == "magnum")
			{
				gunType = GunType::MagnumPistol;
			}
			else if (gunText == "pistol")
			{
				gunType = GunType::PlasmaPistol;
			}
			else if (gunText == "rifle")
			{
				gunType = GunType::PlasmaRifle;
			}
			else if (gunText == "needle")
			{
				gunType = GunType::Needler;
			}
			else
			{
				exit(422);
			}

			m_GunPtrs.push_back(std::make_unique<GunPickup>(m_Textures, Point2f(stof(x),stof(y)), gunType));
		}
	}
}

void Game::ClearEnemies()
{
	m_EnemyPtrs.clear();
}

void Game::ClearGunDrops()
{
	m_GunPtrs.clear();
}

void Game::ClearProjectiles()
{
	m_PlayerProjectilePtrs.clear();
	m_EnemyProjectilePtrs.clear();
}

void Game::HandlePickups(float elapsedSec)
{
	for (int i{}; i < m_GunPtrs.size(); i++)
	{
		m_GunPtrs[i]->Update(elapsedSec, m_Level);
		if (m_GunPtrs[i]->CheckOverlap(m_PlayerPtr->GetShape()))
		{
			PickupInteract(m_GunPtrs[i]->GetGunType(), m_GunPtrs[i]->GetAmmo(), m_GunPtrs[i]->GetReserve());
			if (m_PickedUp)
			{
				m_GunPtrs.erase(m_GunPtrs.begin() + i);
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
				m_GunPtrs.push_back(std::make_unique<GunPickup>(m_Textures, m_PlayerPtr->GetDropPoint(), m_PlayerPtr->GetActiveGun(), m_PlayerPtr->GetAmmo(), m_PlayerPtr->GetReserve()));
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
	// Handle player interactions
	if (m_PlayerPtr->IsShooting())
	{
		NewPlayerProjectile(m_PlayerPtr->GetAngle());
	}
	if (m_PlayerPtr->IsMelee())
	{
		for (int i{}; i < m_EnemyPtrs.size(); i++)
		{
			if (utils::IsOverlapping(m_EnemyPtrs[i]->GetShape(), m_PlayerPtr->GetShape()) && !m_EnemyPtrs[i]->IsDead())
			{
				m_PlayerPtr->PlayMeleeSound();
				m_EnemyPtrs[i]->TakeDamage(50);
			}
		}
	}

	// Handle enemy interactions
	for (int i{}; i < m_EnemyPtrs.size(); i++)
	{
		if (m_EnemyPtrs[i]->IsShooting())
		{
			const Point2f aimPoint
			{ 
				m_PlayerPtr->GetShape().left - m_EnemyPtrs[i]->GetShape().left, 
				m_PlayerPtr->GetShape().bottom - m_EnemyPtrs[i]->GetShape().bottom
			};
			const float enemyAngle{ atan2f(aimPoint.y, aimPoint.x)};
			NewEnemyProjectile(enemyAngle, i);
		}
		if (m_EnemyPtrs[i]->IsMelee())
		{
			if (utils::IsOverlapping(m_EnemyPtrs[i]->GetShape(), m_PlayerPtr->GetShape()))
			{
				m_PlayerPtr->PlayMeleeSound();
				m_PlayerPtr->TakeDamage(60);
			}
		}
	}
	
	const Point2f playerCameraPos{ m_Camera.CameraPos(m_PlayerPtr->GetShape()) };

	// Handle player projectiles
	for (int i{}; i < m_PlayerProjectilePtrs.size(); i++)
	{
		m_PlayerProjectilePtrs[i]->UpdatePosition(elapsedSec, m_Level);

		for (int j{}; j < m_EnemyPtrs.size(); j++)
		{
			if (m_PlayerProjectilePtrs[i]->CheckHit(m_EnemyPtrs[j]->GetShape()))
			{
				m_EnemyPtrs[j]->TakeDamage(m_PlayerProjectilePtrs[i]->GetDamage());
			}
		}
		// Delete if projectile made impact or is out of screen
		if (m_PlayerProjectilePtrs[i]->NeedsDeletion() 
			|| utils::IsOverlapping(m_PlayerProjectilePtrs[i]->GetHitBox(), 
				Rectf(playerCameraPos.x, playerCameraPos.y, GetViewPort().width, GetViewPort().height)) == false)
		{
			m_PlayerProjectilePtrs.erase(m_PlayerProjectilePtrs.begin() + i);
			break;
		}
	}

	// Handle enemy projectiles
	for (int i{}; i < m_EnemyProjectilePtrs.size(); i++)
	{
		m_EnemyProjectilePtrs[i]->UpdatePosition(elapsedSec, m_Level);

		if (m_EnemyProjectilePtrs[i]->CheckHit(m_PlayerPtr->GetShape()))
		{
			m_PlayerPtr->TakeDamage(m_EnemyProjectilePtrs[i]->GetDamage());
		}

		// Delete if projectile made impact or is out of screen
		if (m_EnemyProjectilePtrs[i]->NeedsDeletion() 
			|| utils::IsOverlapping(m_EnemyProjectilePtrs[i]->GetHitBox(), 
				Rectf(playerCameraPos.x, playerCameraPos.y, GetViewPort().width, GetViewPort().height)) == false)
		{
			m_EnemyProjectilePtrs.erase(m_EnemyProjectilePtrs.begin() + i);
			break;
		}
	}

}

void Game::NewPlayerProjectile(float angle)
{
	m_PlayerProjectilePtrs.push_back(std::make_unique<Projectile>(m_Textures, 
		Point2f(m_PlayerPtr->GetShape().left + m_PlayerPtr->GetShape().width / 2
			, m_PlayerPtr->GetShape().bottom + m_PlayerPtr->GetShape().height * 2.f / 3.f), 
		angle, m_PlayerPtr->GetActiveGun(), m_PlayerPtr->GetFlipped()));
}

void Game::NewEnemyProjectile(float angle, int idx)
{
	m_EnemyProjectilePtrs.push_back(std::make_unique<Projectile>(m_Textures,
		Point2f(m_EnemyPtrs[idx]->GetShape().left + m_EnemyPtrs[idx]->GetShape().width / 2
			, m_EnemyPtrs[idx]->GetShape().bottom + m_EnemyPtrs[idx]->GetShape().height / 2), 
		angle, m_EnemyPtrs[idx]->GetWeapon(), m_EnemyPtrs[idx]->GetFlipped()));
}

void Game::HandleEnemyDrop()
{
	for (int i{}; i < m_EnemyPtrs.size(); i++)
	{
		if (m_EnemyPtrs[i]->DropWeapon())
		{
			GunType gunType{};
			switch (m_EnemyPtrs[i]->GetWeapon())
			{
			case EnemyGunType::PlasmaPistol:
				gunType = GunType::PlasmaPistol;
				break;
			case EnemyGunType::PlasmaRifle:
				gunType = GunType::PlasmaRifle;
				break;
			case EnemyGunType::Needler:
				gunType = GunType::Needler;
				break;
			default:
				break;
			}
			
			m_GunPtrs.push_back(std::make_unique<GunPickup>(m_Textures, m_EnemyPtrs[i]->GetDropPoint(), gunType));
		}
	}
}