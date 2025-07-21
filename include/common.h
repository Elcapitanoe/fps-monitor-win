#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi.h>
#include <gl/GL.h>

// Forward declarations to avoid full includes
struct ID3DXFont;
#include <psapi.h>
#include <shlwapi.h>

#include <string>
#include <memory>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

// Application constants
#define APP_NAME L"FPS Overlay"
#define CONFIG_FILE L"config.ini"
#define DEFAULT_FONT_SIZE 16
#define DEFAULT_UPDATE_INTERVAL 500  // milliseconds
#define MAX_MEMORY_USAGE (25 * 1024 * 1024)  // 25MB

// FPS calculation
#define FPS_SAMPLE_COUNT 60
#define MIN_FRAME_TIME 0.001f  // 1ms minimum

// Overlay positioning
enum class OverlayPosition {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3
};

// Graphics API types
enum class GraphicsAPI {
    UNKNOWN = 0,
    D3D9 = 1,
    D3D11 = 2,
    D3D12 = 3,
    OPENGL = 4,
    VULKAN = 5
};

// Color structure
struct Color {
    float r, g, b, a;
    Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};

// Configuration structure
struct OverlayConfig {
    bool enabled = true;
    OverlayPosition position = OverlayPosition::TOP_LEFT;
    int fontSize = DEFAULT_FONT_SIZE;
    Color textColor = Color(0.0f, 1.0f, 0.0f, 1.0f);  // Green
    Color backgroundColor = Color(0.0f, 0.0f, 0.0f, 0.5f);  // Semi-transparent black
    int updateInterval = DEFAULT_UPDATE_INTERVAL;
    int offsetX = 10;
    int offsetY = 10;
    bool showBackground = true;
    std::wstring fontName = L"Consolas";
};

// Utility macros
#define SAFE_RELEASE(p) do { if(p) { (p)->Release(); (p) = nullptr; } } while(0)
#define LOG_ERROR(msg) OutputDebugStringA("[FPS Overlay Error] " msg "\n")
#define LOG_INFO(msg) OutputDebugStringA("[FPS Overlay Info] " msg "\n")

// Function signatures for hooks
typedef HRESULT(WINAPI* D3D9Present_t)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(WINAPI* D3D11Present_t)(IDXGISwapChain*, UINT, UINT);
typedef BOOL(WINAPI* SwapBuffers_t)(HDC);

// Global state
extern std::atomic<bool> g_running;
extern std::atomic<float> g_currentFPS;
extern std::mutex g_configMutex;