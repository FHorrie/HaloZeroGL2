#include "pch.h"
#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager()
	: m_pSoundStreamMap{}
	, m_pSoundEffectMap{}
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
	if (!effect)
		m_pSoundStreamMap.insert({ key, std::make_unique<SoundStream>(path) });
	else
		m_pSoundEffectMap.insert({ key, std::make_unique<SoundEffect>(path) });
}

const SoundStream* SoundManager::GetSoundStream(const std::string& key) const
{
	auto streamIt = m_pSoundStreamMap.find(key);

	if (streamIt != m_pSoundStreamMap.cend())
		return streamIt->second.get();
	else
	{
		std::cout << "Provided string doesn't match any soundstream\nNo soundstream was loaded\n\n";
		return nullptr;
	}
}

const SoundEffect* SoundManager::GetSoundEffect(const std::string& key) const
{
	auto effectIt = m_pSoundEffectMap.find(key);

	if (effectIt != m_pSoundEffectMap.cend())
		return effectIt->second.get();
	else
	{
		std::cout << "Provided string doesn't match any soundeffect\nNo soundeffect was loaded\n\n";
		return nullptr;
	}
}