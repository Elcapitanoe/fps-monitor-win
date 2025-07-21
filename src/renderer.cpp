#include "renderer.h"
#include "utils.h"
#include <sstream>
#include <iomanip>

// Window class name for overlay
#define OVERLAY_CLASS_NAME L"FPSOverlayWindow"

Renderer::Renderer()
    : m_initialized(false)
    , m_currentAPI(GraphicsAPI::UNKNOWN)
    , m_screenWidth(0)
    , m_screenHeight(0)
    , m_d3d9Device(nullptr)
    , m_d3d9Font(nullptr)
    , m_d3d11Device(nullptr)
    , m_d3d11Context(nullptr)
    , m_d3d11RenderTarget(nullptr)
    , m_glHDC(nullptr)
    , m_glContext(nullptr)
    , m_glFont(nullptr)
    , m_font(nullptr)
    , m_overlayWindow(nullptr)
{
}

Renderer::~Renderer() {
    Cleanup();
}

bool Renderer::Initialize(GraphicsAPI api, void* device) {
    if (m_initialized) {
        Cleanup();
    }
    
    m_currentAPI = api;
    UpdateScreenDimensions(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    
    Utils::LogInfo(L"Initializing renderer");
    
    // For maximum compatibility, we'll use a layered window overlay approach
    // This works across all graphics APIs and avoids hooking issues
    if (!CreateOverlayWindow()) {
        Utils::LogError(L"Failed to create overlay window");
        return false;
    }
    
    m_initialized = true;
    Utils::LogInfo(L"Renderer initialized successfully");
    return true;
}

void Renderer::Cleanup() {
    if (!m_initialized) return;
    
    Utils::LogInfo(L"Cleaning up renderer");
    
    // Cleanup DirectX resources
    if (m_d3d9Font) {
        // Would release ID3DXFont here if using D3DX
        m_d3d9Font = nullptr;
    }
    SAFE_RELEASE(m_d3d9Device);
    SAFE_RELEASE(m_d3d11RenderTarget);
    SAFE_RELEASE(m_d3d11Context);
    SAFE_RELEASE(m_d3d11Device);
    
    // Cleanup OpenGL resources
    if (m_glContext) {
        wglDeleteContext(m_glContext);
        m_glContext = nullptr;
    }
    if (m_glHDC) {
        ReleaseDC(nullptr, m_glHDC);
        m_glHDC = nullptr;
    }
    
    // Cleanup GDI resources
    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }
    if (m_glFont) {
        DeleteObject(m_glFont);
        m_glFont = nullptr;
    }
    
    // Cleanup overlay window
    DestroyOverlayWindow();
    
    m_initialized = false;
    Utils::LogInfo(L"Renderer cleanup completed");
}

void Renderer::RenderOverlay(float fps, const OverlayConfig& config) {
    if (!m_initialized || !m_overlayWindow) return;
    
    // Format FPS text
    std::wstring fpsText = FormatFPS(fps);
    
    // Update overlay window position and content
    HDC hdc = GetDC(m_overlayWindow);
    if (!hdc) return;
    
    // Create compatible bitmap for layered window
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 200, 50);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
    
    // Clear background
    RECT rect = {0, 0, 200, 50};
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memDC, &rect, hBrush);
    DeleteObject(hBrush);
    
    // Create font
    CreateFont(config.fontName, config.fontSize);
    HFONT hOldFont = (HFONT)SelectObject(memDC, m_font);
    
    // Set text properties
    SetTextColor(memDC, RGB(
        (BYTE)(config.textColor.r * 255),
        (BYTE)(config.textColor.g * 255),
        (BYTE)(config.textColor.b * 255)
    ));
    SetBkMode(memDC, TRANSPARENT);
    
    // Draw text
    DrawTextW(memDC, fpsText.c_str(), -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
    
    // Calculate position
    int x, y, width, height;
    GetTextPosition(config, fpsText, x, y, width, height);
    
    // Update layered window
    POINT ptSrc = {0, 0};
    POINT ptDst = {x, y};
    SIZE sizeWnd = {width, height};
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = (BYTE)(config.textColor.a * 255);
    blend.AlphaFormat = 0;
    
    UpdateLayeredWindow(m_overlayWindow, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    
    // Cleanup
    SelectObject(memDC, hOldFont);
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(m_overlayWindow, hdc);
}

void Renderer::UpdateScreenDimensions(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

bool Renderer::CreateOverlayWindow() {
    // Register window class
    WNDCLASSEXW wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = OverlayWndProc;
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = OVERLAY_CLASS_NAME;
    
    RegisterClassExW(&wcex);
    
    // Create layered window
    m_overlayWindow = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        OVERLAY_CLASS_NAME,
        L"FPS Overlay",
        WS_POPUP,
        0, 0, 200, 50,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        this
    );
    
    if (!m_overlayWindow) {
        Utils::LogError(L"Failed to create overlay window: " + Utils::GetLastErrorString());
        return false;
    }
    
    // Set window properties
    SetWindowPos(m_overlayWindow, HWND_TOPMOST, 0, 0, 0, 0, 
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    
    ShowWindow(m_overlayWindow, SW_SHOW);
    
    return true;
}

void Renderer::DestroyOverlayWindow() {
    if (m_overlayWindow) {
        DestroyWindow(m_overlayWindow);
        m_overlayWindow = nullptr;
    }
    
    UnregisterClassW(OVERLAY_CLASS_NAME, GetModuleHandle(nullptr));
}

LRESULT CALLBACK Renderer::OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        {
            CREATESTRUCT* pcs = (CREATESTRUCT*)lParam;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
        }
        return 0;
        
    case WM_DESTROY:
        return 0;
        
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // Painting is handled by UpdateLayeredWindow
            EndPaint(hwnd, &ps);
        }
        return 0;
        
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

// Helper function implementations
void Renderer::CreateFont(const std::wstring& fontName, int fontSize) {
    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }
    
    m_font = CreateFontW(
        fontSize,                    // height
        0,                          // width
        0,                          // escapement
        0,                          // orientation
        FW_NORMAL,                  // weight
        FALSE,                      // italic
        FALSE,                      // underline
        FALSE,                      // strikeout
        DEFAULT_CHARSET,            // charset
        OUT_DEFAULT_PRECIS,         // output precision
        CLIP_DEFAULT_PRECIS,        // clipping precision
        CLEARTYPE_QUALITY,          // quality
        DEFAULT_PITCH | FF_DONTCARE,// pitch and family
        fontName.c_str()            // face name
    );
    
    if (!m_font) {
        Utils::LogWarning(L"Failed to create font: " + fontName);
        // Create default font
        m_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }
}

void Renderer::GetTextPosition(const OverlayConfig& config, const std::wstring& text,
                              int& x, int& y, int& width, int& height) {
    // Calculate text dimensions
    HDC hdc = GetDC(nullptr);
    if (hdc) {
        HFONT hOldFont = (HFONT)SelectObject(hdc, m_font);
        
        SIZE textSize;
        GetTextExtentPoint32W(hdc, text.c_str(), (int)text.length(), &textSize);
        
        width = textSize.cx + 20;  // Add padding
        height = textSize.cy + 10; // Add padding
        
        SelectObject(hdc, hOldFont);
        ReleaseDC(nullptr, hdc);
    } else {
        width = 100;
        height = 30;
    }
    
    // Calculate position based on overlay position setting
    switch (config.position) {
    case OverlayPosition::TOP_LEFT:
        x = config.offsetX;
        y = config.offsetY;
        break;
        
    case OverlayPosition::TOP_RIGHT:
        x = m_screenWidth - width - config.offsetX;
        y = config.offsetY;
        break;
        
    case OverlayPosition::BOTTOM_LEFT:
        x = config.offsetX;
        y = m_screenHeight - height - config.offsetY;
        break;
        
    case OverlayPosition::BOTTOM_RIGHT:
        x = m_screenWidth - width - config.offsetX;
        y = m_screenHeight - height - config.offsetY;
        break;
        
    default:
        x = config.offsetX;
        y = config.offsetY;
        break;
    }
    
    // Ensure position is within screen bounds
    x = std::max(0, std::min(x, m_screenWidth - width));
    y = std::max(0, std::min(y, m_screenHeight - height));
}

DWORD Renderer::ColorToD3DColor(const Color& color) {
    return D3DCOLOR_ARGB(
        (DWORD)(color.a * 255),
        (DWORD)(color.r * 255),
        (DWORD)(color.g * 255),
        (DWORD)(color.b * 255)
    );
}

std::wstring Renderer::FormatFPS(float fps) {
    std::wostringstream oss;
    oss << L"FPS: " << std::fixed << std::setprecision(1) << fps;
    return oss.str();
}

// Graphics API specific implementations (simplified for this version)
bool Renderer::InitializeD3D9(IDirect3DDevice9* device) {
    m_d3d9Device = device;
    if (m_d3d9Device) {
        m_d3d9Device->AddRef();
    }
    return true;
}

bool Renderer::InitializeD3D11(ID3D11Device* device) {
    m_d3d11Device = device;
    if (m_d3d11Device) {
        m_d3d11Device->AddRef();
        m_d3d11Device->GetImmediateContext(&m_d3d11Context);
    }
    return true;
}

bool Renderer::InitializeOpenGL(HDC hdc) {
    m_glHDC = hdc;
    return true;
}

void Renderer::RenderD3D9(const std::wstring& text, const OverlayConfig& config) {
    // DirectX 9 specific rendering would go here
    // For now, fallback to window overlay
}

void Renderer::RenderD3D11(const std::wstring& text, const OverlayConfig& config) {
    // DirectX 11 specific rendering would go here
    // For now, fallback to window overlay
}

void Renderer::RenderOpenGL(const std::wstring& text, const OverlayConfig& config) {
    // OpenGL specific rendering would go here
    // For now, fallback to window overlay
}