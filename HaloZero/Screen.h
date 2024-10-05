#pragma once
#include "StaticSprite.h"

class Screen : public StaticSprite
{
public:
	explicit Screen(const std::string& spriteType, const StaticTextures& textures);
	Screen(const Screen& other) = delete;
	Screen& operator=(const Screen& other) = delete;
	Screen(Screen&& other) = delete;
	Screen& operator=(Screen && other) = delete;
	~Screen() = default;

};