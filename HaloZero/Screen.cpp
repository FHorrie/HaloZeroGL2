#include "pch.h"
#include "Screen.h"

Screen::Screen(const std::string& screenType, const StaticTextures& textures)
	:StaticSprite(screenType, textures, Point2f{0.f, 0.f})
{
}