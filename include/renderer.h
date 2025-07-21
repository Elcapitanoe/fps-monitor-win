#pragma once

#include "common.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialize renderer for specific graphics API
    bool Initialize(GraphicsAPI api, void* device = nullptr);
    
    // Cleanup renderer resources
    void Cleanup();
    
    // Render FPS overlay
    void RenderOverlay(float fps, const OverlayConfig& config);
    
    // Check if renderer is ready
    bool IsInitialized() const { return m_initialized; }
    
    // Get current graphics API
    GraphicsAPI GetCurrentAPI() const { return m_currentAPI; }
    
    // Update screen dimensions
    void UpdateScreenDimensions(int width, int height);

private:
    bool m_initialized;
    GraphicsAPI m_currentAPI;
    int m_screenWidth;
    int m_screenHeight;
    
    // DirectX 9 resources
    IDirect3DDevice9* m_d3d9Device;
    void* m_d3d9Font;  // ID3DXFont* (using void* to avoid D3DX dependency)
    
    // DirectX 11 resources
    ID3D11Device* m_d3d11Device;
    ID3D11DeviceContext* m_d3d11Context;
    ID3D11RenderTargetView* m_d3d11RenderTarget;
    
    // OpenGL resources
    HDC m_glHDC;
    HGLRC m_glContext;
    HFONT m_glFont;
    
    // Common resources
    HFONT m_font;
    
    // Initialization functions for different APIs
    bool InitializeD3D9(IDirect3DDevice9* device);
    bool InitializeD3D11(ID3D11Device* device);
    bool InitializeOpenGL(HDC hdc);
    
    // Rendering functions for different APIs
    void RenderD3D9(const std::wstring& text, const OverlayConfig& config);
    void RenderD3D11(const std::wstring& text, const OverlayConfig& config);
    void RenderOpenGL(const std::wstring& text, const OverlayConfig& config);
    
    // Helper functions
    void CreateFont(const std::wstring& fontName, int fontSize);
    void GetTextPosition(const OverlayConfig& config, const std::wstring& text, 
                        int& x, int& y, int& width, int& height);
    DWORD ColorToD3DColor(const Color& color);
    std::wstring FormatFPS(float fps);
    
    // Screen overlay for fallback rendering
    HWND m_overlayWindow;
    bool CreateOverlayWindow();
    void DestroyOverlayWindow();
    static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};