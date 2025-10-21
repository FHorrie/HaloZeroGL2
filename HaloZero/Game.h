#pragma once
#include "BaseGame.h"
#include "StaticTextures.h"
#include "SoundManager.h"
#include "Level.h"
#include "Camera.h"
#include "HUD.h"
#include "Screen.h"
#include "MasterChiefBase.h"

class MasterChiefLegs;
class MasterChiefTorso;
class AnimatedSprite;
class GunPickup;
class EnemyBase;
class Projectile;
class EnvironmentSprite;

class Game final : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup();

	void CleanScreens();

	void DrawEntities() const;
	void DrawScreens() const;

	void ReInit();

	void ClearBackground( ) const;
	void PrintControls() const;
	void SetCamera();
	void UpdateHUD();

	void PlayLevelBGM() const;
	void PlayWinBGM() const;
	void PlayDeathBGM() const;

	void LoadElements(std::string fileLocation);
	void ClearEnemies();
	void ClearGunDrops();
	void ClearProjectiles();
	
	void HandlePickups(float elapsedSec);
	void PickupInteract(GunType type, int ammo, int reserve);

	void HandleInteractions(float elapsedSec);
	void NewPlayerProjectile(float angle);
	void NewEnemyProjectile(float angle, int idx);

	void HandleEnemyDrop();

	// VARIABLES
	StaticTextures m_Textures{};
	SoundManager m_Sounds{};

	Screen m_ControlsScreen;
	Screen m_EndScreen;
	Screen m_DeathScreen;

	std::unique_ptr<MasterChiefBase> m_PlayerPtr{};
	std::unique_ptr<EnvironmentSprite> m_pPlayerDeath{};

	Level m_Level;
	Camera m_Camera;
	HUD m_HUD;

	std::vector<std::unique_ptr<EnemyBase>> m_EnemyPtrs;

	std::vector<std::unique_ptr<GunPickup>> m_GunPtrs;

	std::vector<std::unique_ptr<Projectile>> m_PlayerProjectilePtrs;
	std::vector<std::unique_ptr<Projectile>> m_EnemyProjectilePtrs;

	bool m_PickedUp{};
	bool m_ButtonPressed{};
		 
	bool m_DisplayControls{};
	bool m_GameEnd{};
	bool m_GameDead{};
	bool m_FadeOut{};
	float m_EndAlpha{};

	//SoundStream*			m_pBGM;
	//SoundStream*			m_pEndMusic;

	const std::string m_GameInfoText{ "\n-- Halo Zero Controls --\nLMB = Shoot\nRMB = Melee\nA, D = Move\nS = Crouch\nSpacebar = Jump\n1, 2 = Switch Weapons\nLSHIFT = Pick Up Weapon\nG = Grenade\n\n" };
};