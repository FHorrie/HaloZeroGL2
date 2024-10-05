#pragma once
#include "Texture.h"
#include <map>

class StaticTextures final
{
public:
	explicit StaticTextures();
	StaticTextures(const StaticTextures& other) = delete;
	StaticTextures& operator= (const StaticTextures& other) = delete;
	StaticTextures(const StaticTextures&& other) = delete;
	StaticTextures& operator= (const StaticTextures&& other) = delete;
	~StaticTextures();

	const Texture* GetTexture(const std::string& key) const;

private:
	std::map<std::string, Texture*> m_pTextureMap;

	void LoadTexture(const std::string& key, const std::string& path);

};

