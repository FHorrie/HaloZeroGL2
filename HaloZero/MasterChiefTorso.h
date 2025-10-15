#pragma once
#include "AnimatedSprite.h"

class MasterChiefBase;

class MasterChiefTorso final : public AnimatedSprite
{
public:
	explicit MasterChiefTorso(const StaticTextures& textures, 
		const Point2f& startLocation, unsigned short framerate, MasterChiefBase* const base);
	MasterChiefTorso(const MasterChiefTorso& other) = delete;
	MasterChiefTorso& operator= (const MasterChiefTorso& other) = delete;
	MasterChiefTorso(MasterChiefTorso&& other) = delete;
	MasterChiefTorso& operator=(MasterChiefTorso&& other) = delete;
	~MasterChiefTorso() = default;

	void Draw() const override;
	void Update(float elapsedSec);

private:
	unsigned short m_RowOffset{};
	unsigned short m_CurrentRow{};

	MasterChiefBase* const m_BasePtr{};
	
	void UpdateFramesState();
	void UpdateCurrentFrame(float elapsedSec);
};

