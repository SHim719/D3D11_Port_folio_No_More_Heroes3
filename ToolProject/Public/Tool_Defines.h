#pragma once
#pragma warning (disable : 6031)

#include "../Default/stdafx.h"
#include <process.h>
#include <shobjidl.h>

#include "Tool_Enums.h"
#include "Tool_Struct.h"
#include "Tool_Macro.h"
#include "Tool_Functions.h"


/* 클라이언트에서 제작하는 모든 클래스들이 공통적으로 자주 사용하는 정의들을 모아둔다. */
namespace Tool
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL {
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_ANIMTOOL,
		LEVEL_MAPTOOL,
		LEVEL_UITOOL,
		LEVEL_EFFECTTOOL,
		LEVEL_CINEMATOOL,
		LEVEL_END
	};

	enum ToolType {
		ANIM,
		MAP, 
		UI,
		EFFECT,
		CINEMATIC,
		TOOL_END,
	};




}

extern HWND		g_hWnd;

using namespace Tool;