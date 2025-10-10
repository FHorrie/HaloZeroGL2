#include "pch.h"
#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager()
{
	LoadSound("Level1", "Sounds/Music/level1.ogg", false);
	LoadSound("GameDead", "Sounds/Music/gamedead.ogg", false);
	LoadSound("GameWin", "Sounds/Music/gamewin.ogg", false);

	LoadSound("MagnumShot", "Sounds/SoundFX/GunSounds/MagnumShot.ogg", true);
	LoadSound("SmartRifleShot", "Sounds/SoundFX/GunSounds/SmartRifleShot.ogg", true);
	LoadSound("PlasmaPistolShot", "Sounds/SoundFX/GunSounds/PlasmaPistolShot.ogg", true);
	LoadSound("PlasmaRifleShot", "Sounds/SoundFX/GunSounds/PlasmaRifleShot.ogg", true);
	LoadSound("NeedlerShot", "Sounds/SoundFX/GunSounds/NeedlerShot.ogg", true);
	LoadSound("HunterShot", "Sounds/SoundFX/GunSounds/HunterShot.ogg", true);
	LoadSound("ReloadDone", "Sounds/SoundFX/GunSounds/ReloadDone.ogg", true);
	
	LoadSound("ImpDeath", "Sounds/SoundFX/Covenant/ImpDeath.ogg", true);
	LoadSound("EliteDeath", "Sounds/SoundFX/Covenant/EliteDeath.ogg", true);
	LoadSound("JackalDeath", "Sounds/SoundFX/Covenant/JackalDeath.ogg", true);
	LoadSound("HunterDeath", "Sounds/SoundFX/Covenant/HunterDeath.ogg", true);

	LoadSound("ShieldBeep", "Sounds/SoundFX/MasterChief/ShieldBeep.ogg", true);
	LoadSound("ShieldRecharge", "Sounds/SoundFX/MasterChief/ShieldRecharge.ogg", true);
	LoadSound("Melee", "Sounds/SoundFX/MasterChief/Melee.ogg", true);
}

void SoundManager::LoadSound(const std::string& key, const std::string& path, bool effect)
{
	if (effect)
	{
		m_SoundEffectMapPtr.insert({ key, std::make_unique<SoundEffect>(path) });
	}
	else
	{
		m_SoundStreamMapPtr.insert({ key, std::make_unique<SoundStream>(path) });
	}
}

const SoundStream* SoundManager::GetSoundStream(const std::string& key) const
{
	auto streamIt = m_SoundStreamMapPtr.find(key);

	if (streamIt != m_SoundStreamMapPtr.cend())
	{
		return streamIt->second.get();
	}
	else
	{
		std::cout << "Provided string doesn't match any soundstream\nNo soundstream was loaded\n\n";
		return nullptr;
	}
}

const SoundEffect* SoundManager::GetSoundEffect(const std::string& key) const
{
	auto effectIt = m_SoundEffectMapPtr.find(key);

	if (effectIt != m_SoundEffectMapPtr.cend())
	{
		return effectIt->second.get();
	}
	else
	{
		std::cout << "Provided string doesn't match any soundeffect\nNo soundeffect was loaded\n\n";
		return nullptr;
	}
}