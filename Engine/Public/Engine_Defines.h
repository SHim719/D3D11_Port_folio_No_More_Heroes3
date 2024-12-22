#pragma once

#ifdef _ENGINE_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4828)
#pragma warning (disable : 4819)
#pragma warning (disable : 6262)
#pragma warning (disable : 6031)

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/DirectXCollision.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <map>
#include <list>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <string>
#include <assert.h>
#include <iostream>
#include <ctime>
#include <random>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <filesystem>
#include "json.hpp"

#include "Engine_Enums.h"

#define TEXTURE_TYPE_MAX TextureType_FRENSEL_NORMAL + 1

#include "Engine_Enums.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Render_Descs.h"
#include "Engine_Struct.h"

using namespace Engine;
using namespace std;

namespace fs = std::filesystem;
using json = nlohmann::json;

