#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// DirectX
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>

#include <wrl/client.h>

// STL
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <unordered_map>
#include <algorithm>

// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"
#include "MathHelper.h"
#include "PropertyHelper.h"

// Rendering
#include "Shader/ShaderBuffers.h"

using namespace DirectX;