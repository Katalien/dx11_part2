#pragma once

// common libraries
#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <set>

// d3d libraries
#pragma comment(lib,"d3dcompiler")
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi.h>

// math library
#include <directxmath.h>

// Dear Imgui library
#include "imgui/imgui.h"

#define SAFE_RELEASE(a) if (a != NULL) { a->Release(); a = NULL; }

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define ASSERT(x) { if (!(x)) { MessageBoxA(NULL, "ASSERT:" #x "\n" AT, "ASSERT", MB_OK | MB_ICONERROR); __debugbreak(); abort(); } }

// forward declaration
class Engine;
class Render;
