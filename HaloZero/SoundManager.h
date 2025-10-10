#pragma once
#include "SoundStream.h"
#include "SoundEffect.h"
#include <unordered_map>
#include <memory>

class SoundManager
{
public:
	explicit SoundManager();
	~SoundManager() = default;
	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator= (const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) = delete;
	SoundManager& operator= (SoundManager&& other) = delete;

	const SoundStream* GetSoundStream(const std::string& key) const;
	const SoundEffect* GetSoundEffect(const std::string& key) const;

private:
	std::unordered_map<std::string, std::unique_ptr<SoundStream>> m_SoundStreamMapPtr{};
	std::unordered_map<std::string, std::unique_ptr<SoundEffect>> m_SoundEffectMapPtr{};

	void LoadSound(const std::string& key, const std::string& path, bool effect);
};
