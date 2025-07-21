#pragma once

#include "common.h"

class HookManager {
public:
    HookManager();
    ~HookManager();
    
    // Initialize and install hooks
    bool Initialize();
    
    // Cleanup and remove hooks
    void Cleanup();
    
    // Check if hooks are active
    bool IsActive() const { return m_active; }
    
    // Detect current graphics API
    GraphicsAPI DetectGraphicsAPI();
    
    // Get current detected API
    GraphicsAPI GetCurrentAPI() const { return m_detectedAPI; }
    
    // Force refresh hooks (useful when switching applications)
    void RefreshHooks();

private:
    bool m_active;
    GraphicsAPI m_detectedAPI;
    
    // Hook addresses
    void* m_d3d9PresentAddr;
    void* m_d3d11PresentAddr;
    void* m_swapBuffersAddr;
    
    // Original function pointers
    D3D9Present_t m_originalD3D9Present;
    D3D11Present_t m_originalD3D11Present;
    SwapBuffers_t m_originalSwapBuffers;
    
    // Hook installation functions
    bool InstallD3D9Hooks();
    bool InstallD3D11Hooks();
    bool InstallOpenGLHooks();
    
    // Hook removal functions
    void RemoveD3D9Hooks();
    void RemoveD3D11Hooks();
    void RemoveOpenGLHooks();
    
    // Hook functions
    static HRESULT WINAPI D3D9PresentHook(IDirect3DDevice9* device, CONST RECT* pSourceRect,
                                         CONST RECT* pDestRect, HWND hDestWindowOverride,
                                         CONST RGNDATA* pDirtyRegion);
    
    static HRESULT WINAPI D3D11PresentHook(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags);
    
    static BOOL WINAPI SwapBuffersHook(HDC hdc);
    
    // Helper functions
    void* GetProcAddressFromModule(const wchar_t* moduleName, const char* procName);
    bool GetD3D9PresentAddress(void** address);
    bool GetD3D11PresentAddress(void** address);
    bool GetSwapBuffersAddress(void** address);
    
    // Simple inline hooking implementation
    bool InstallHook(void* targetFunction, void* hookFunction, void** originalFunction);
    bool RemoveHook(void* targetFunction, void* originalFunction);
    
    // Process detection
    bool IsTargetProcess();
    std::wstring GetCurrentProcessName();
};