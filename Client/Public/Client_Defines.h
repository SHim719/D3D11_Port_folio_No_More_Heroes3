#pragma once

#include "../Default/stdafx.h"
#include <process.h>

#include "Client_Enums.h"
#include "Client_Struct.h"
#include "Client_Macro.h"
#include "Client_Functions.h"
#include "Client_Components.h"

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL {
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_TEST,

		LEVEL_MAINMENU,
		LEVEL_WORLD_HOME,
		LEVEL_WORLD_HOME_LAB,
		LEVEL_BATTLE_MOTEL_FRONT,
		LEVEL_WORLD_CITY,
		LEVEL_WORLD_CITY_TRASHCOLLECTOR,
		LEVEL_WORLD_CITY_TOILET,
		LEVEL_BATTLE_UFO_OUTSIDE,
		LEVEL_WORLD_UFO,
		LEVEL_WORLD_UFO_TOILET,
		LEVEL_BATTLE_UFO_INSIDE,
		LEVEL_BOSS_BLACKHOLE_GROUND,
		LEVEL_BOSS_BLACKHOLE_SPACE,
		LEVEL_BOSS_SONICJUICE,
		LEVEL_SMASHBROS_BOSS_DAMON,
		LEVEL_ARROW_A_ROW,
		LEVEL_MARIO,
		LEVEL_BATTLE_ISLAND,
		LEVEL_ENDING,

		LEVEL_END
	};

	enum TAG
	{
		TAG_PLAYER = 0,
		TAG_PLAYER_WEAPON,
		TAG_NPC,
		TAG_TRIGGER,
		TAG_ENEMY,
		TAG_ENEMY_WEAPON,
		TAG_BOSS,
		TAG_BOSS_ATTACK,
		TAG_BOSS_BLACKHOLE,
		TAG_CRYSTAL,
		TAG_PHYSICAL_TERRAIN,
		TAG_PHYSICAL_WALL,
		TAG_PHYSICAL_BLOCK,
		TAG_PHYSICAL_COIN,
		TAG_PHYSICAL_STAR,
		TAG_FRAGILE_BLOCK,
		TAG_END,
	};

	const float g_fSlowFactor_Normal	= 1.f;
	const float g_fSlowFactor_Slow0		= 0.1f;
}

extern HWND				g_hWnd;
extern int g_iDeltaWheel;

using namespace Client;