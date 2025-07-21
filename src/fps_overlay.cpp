#include "fps_overlay.h"
#include "utils.h"
#include <iostream>

FPSOverlay::FPSOverlay()
    : m_running(false)
    , m_initialized(false)
    , m_currentFPS(0.0f)
    , m_frameTimeIndex(0)
    , m_memoryUsage(0)
{
    // Initialize frame time buffer
    m_frameTimes.resize(FPS_SAMPLE_COUNT, 0.0f);
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_lastUpdateTime = std::chrono::high_resolution_clock::now();
    
    // Create component managers
    m_configManager = std::make_unique<ConfigManager>();
    m_hookManager = std::make_unique<HookManager>();
    m_renderer = std::make_unique<Renderer>();
}

FPSOverlay::~FPSOverlay() {
    Stop();
}

bool FPSOverlay::Initialize() {
    if (m_initialized) return true;
    
    Utils::LogInfo(L"Initializing FPS Overlay");
    
    // Check system compatibility
    if (!CheckSystemCompatibility()) {
        Utils::LogError(L"System compatibility check failed");
        return false;
    }
    
    // Setup exception handling
    SetupExceptionHandling();
    
    // Load configuration
    if (!m_configManager->LoadConfig()) {
        Utils::LogWarning(L"Failed to load configuration, using defaults");
    }
    
    // Initialize hook manager
    if (!m_hookManager->Initialize()) {
        Utils::LogWarning(L"Hook manager initialization failed, using fallback FPS calculation");
    }
    
    // Initialize renderer
    GraphicsAPI detectedAPI = m_hookManager->GetCurrentAPI();
    if (!m_renderer->Initialize(detectedAPI)) {
        Utils::LogError(L"Failed to initialize renderer");
        return false;
    }
    
    m_initialized = true;
    Utils::LogInfo(L"FPS Overlay initialized successfully");
    return true;
}

bool FPSOverlay::Start() {
    if (!m_initialized) {
        Utils::LogError(L"Cannot start FPS overlay - not initialized");
        return false;
    }
    
    if (m_running) {
        Utils::LogWarning(L"FPS overlay is already running");
        return true;
    }
    
    Utils::LogInfo(L"Starting FPS Overlay");
    
    m_running = true;
    g_running = true;
    
    // Start update thread
    m_updateThread = std::thread(&FPSOverlay::UpdateWorker, this);
    
    Utils::LogInfo(L"FPS Overlay started successfully");
    return true;
}

void FPSOverlay::Stop() {
    if (!m_running) return;
    
    Utils::LogInfo(L"Stopping FPS Overlay");
    
    m_running = false;
    g_running = false;
    
    // Wait for update thread to finish
    if (m_updateThread.joinable()) {
        m_updateThread.join();
    }
    
    // Cleanup components
    if (m_renderer) {
        m_renderer->Cleanup();
    }
    
    if (m_hookManager) {
        m_hookManager->Cleanup();
    }
    
    Utils::LogInfo(L"FPS Overlay stopped");
}

void FPSOverlay::UpdateFPS() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
    
    m_lastFrameTime = currentTime;
    
    // Calculate FPS using rolling average
    CalculateFPS(deltaTime);
}

float FPSOverlay::GetCurrentFPS() const {
    std::lock_guard<std::mutex> lock(m_fpsMutex);
    return m_currentFPS;
}

bool FPSOverlay::ProcessCommandLine(int argc, wchar_t* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];
        
        if (arg == L"--help" || arg == L"-h" || arg == L"/?") {
            ShowHelp();
            return false;
        }
        else if (arg == L"--version" || arg == L"-v") {
            ShowVersion();
            return false;
        }
        else if (arg == L"--exit") {
            // Terminate any running instance
            return false;
        }
        else if (arg == L"--config" && i + 1 < argc) {
            // Load custom config file
            std::wstring configPath = argv[++i];
            if (!m_configManager->LoadConfig(configPath)) {
                std::wcout << L"Failed to load config file: " << configPath << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

void FPSOverlay::Update() {
    if (!m_running || !m_initialized) return;
    
    // Update FPS calculation continuously (this gets called frequently)
    UpdateFPS();
    
    // Only do heavy operations less frequently
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - m_lastUpdateTime).count();
    
    // Update memory usage and other heavy operations less frequently
    if (timeSinceLastUpdate >= 1000) { // Every 1 second
        MonitorMemoryUsage();
        m_lastUpdateTime = currentTime;
        
        // Refresh hooks if needed
        if (m_hookManager && m_hookManager->IsActive()) {
            m_hookManager->RefreshHooks();
        }
    }
    
    // Render overlay (this should be lightweight)
    if (m_renderer && m_renderer->IsInitialized()) {
        const OverlayConfig& config = m_configManager->GetConfig();
        if (config.enabled) {
            m_renderer->RenderOverlay(GetCurrentFPS(), config);
        }
    }
}

// Private methods implementation
void FPSOverlay::UpdateWorker() {
    Utils::LogInfo(L"FPS Overlay update thread started");
    
    // Use a high frequency update for proper FPS detection (16ms ≈ 60fps)
    const int highFrequencyInterval = 16; // milliseconds
    
    while (m_running) {
        try {
            Update();
            
            // Sleep for a short interval to allow proper frame detection
            std::this_thread::sleep_for(std::chrono::milliseconds(highFrequencyInterval));
            
        } catch (const std::exception& e) {
            Utils::LogError(L"Exception in update worker: " + Utils::Utf8ToWide(e.what()));
            break;
        } catch (...) {
            Utils::LogError(L"Unknown exception in update worker");
            break;
        }
    }
    
    Utils::LogInfo(L"FPS Overlay update thread stopped");
}

void FPSOverlay::CalculateFPS(float deltaTime) {
    std::lock_guard<std::mutex> lock(m_fpsMutex);
    
    // Clamp delta time to prevent division by zero and handle spikes
    deltaTime = std::max(deltaTime, MIN_FRAME_TIME);
    
    // Skip extremely small delta times that might be from too-frequent polling
    if (deltaTime < 0.008f) { // Less than 8ms (125fps+)
        return; // Don't update FPS for very small intervals
    }
    
    // Store frame time in circular buffer
    m_frameTimes[m_frameTimeIndex] = deltaTime;
    m_frameTimeIndex = (m_frameTimeIndex + 1) % FPS_SAMPLE_COUNT;
    
    // Calculate average frame time from recent samples
    float totalTime = 0.0f;
    int validSamples = 0;
    
    for (size_t i = 0; i < m_frameTimes.size(); ++i) {
        if (m_frameTimes[i] > 0.0f) {
            totalTime += m_frameTimes[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        float averageFrameTime = totalTime / validSamples;
        float newFPS = 1.0f / averageFrameTime;
        
        // Smooth the FPS to reduce jitter
        if (m_currentFPS > 0.0f) {
            m_currentFPS = m_currentFPS * 0.9f + newFPS * 0.1f; // Exponential moving average
        } else {
            m_currentFPS = newFPS;
        }
        
        // Clamp FPS to reasonable range
        m_currentFPS = std::max(0.1f, std::min(m_currentFPS, 9999.0f));
    }
    
    // Update global FPS counter
    g_currentFPS = m_currentFPS;
}

void FPSOverlay::MonitorMemoryUsage() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeSinceLastCheck = std::chrono::duration_cast<std::chrono::seconds>(
        currentTime - m_lastUpdateTime).count();
    
    // Check memory usage every 5 seconds
    if (timeSinceLastCheck >= 5) {
        m_memoryUsage = Utils::GetProcessMemoryUsage();
        m_lastUpdateTime = currentTime;
        
        // Check if memory usage exceeds limit
        size_t memoryUsageMB = m_memoryUsage / (1024 * 1024);
        if (memoryUsageMB > MAX_MEMORY_USAGE / (1024 * 1024)) {
            Utils::LogWarning(L"Memory usage exceeds limit: " + std::to_wstring(memoryUsageMB) + L"MB");
        }
    }
}

bool FPSOverlay::CheckSystemCompatibility() {
    // Check Windows version
    if (!Utils::IsWindows7OrLater()) {
        Utils::LogError(L"Windows 7 or later required");
        return false;
    }
    
    // Check available graphics APIs
    auto availableAPIs = Utils::GetAvailableGraphicsAPIs();
    if (availableAPIs.empty()) {
        Utils::LogError(L"No compatible graphics APIs found");
        return false;
    }
    
    // Log available APIs
    std::wstring apiList = L"Available graphics APIs: ";
    for (auto api : availableAPIs) {
        switch (api) {
            case GraphicsAPI::D3D9:  apiList += L"DirectX9 "; break;
            case GraphicsAPI::D3D11: apiList += L"DirectX11 "; break;
            case GraphicsAPI::OPENGL: apiList += L"OpenGL "; break;
            default: break;
        }
    }
    Utils::LogInfo(apiList);
    
    return true;
}

void FPSOverlay::SetupExceptionHandling() {
    // Exception handling is set up in main.cpp
    Utils::LogInfo(L"Exception handling configured");
}

// Command line processing implementation
void FPSOverlay::ShowHelp() {
    std::wcout << L"FPS Overlay v1.0.0 - Real-time FPS monitoring for Windows\n\n";
    std::wcout << L"Usage: FPSOverlay.exe [options]\n\n";
    std::wcout << L"Options:\n";
    std::wcout << L"  --help, -h, /?        Show this help message\n";
    std::wcout << L"  --version, -v         Show version information\n";
    std::wcout << L"  --config <file>       Use custom configuration file\n";
    std::wcout << L"  --exit                Terminate any running instance\n\n";
    std::wcout << L"Configuration:\n";
    std::wcout << L"  Edit 'config.ini' to customize overlay appearance and behavior.\n\n";
    std::wcout << L"Supported Graphics APIs:\n";
    std::wcout << L"  - DirectX 9/11/12\n";
    std::wcout << L"  - OpenGL\n";
    std::wcout << L"  - Vulkan (basic support)\n\n";
    std::wcout << L"Compatible with Windows 7, 8, 10, and 11 (32-bit and 64-bit)\n";
}

void FPSOverlay::ShowVersion() {
    std::wcout << L"FPS Overlay v1.0.0\n";
    std::wcout << L"Built for Windows 7+ (32-bit/64-bit)\n";
    std::wcout << L"Copyright (c) 2024\n\n";
    std::wcout << L"System Information:\n";
    std::wcout << L"  OS: " << Utils::GetWindowsVersion() << L"\n";
    std::wcout << L"  Architecture: " << (sizeof(void*) == 8 ? L"64-bit" : L"32-bit") << L"\n";
    std::wcout << L"  Elevated: " << (Utils::IsProcessElevated() ? L"Yes" : L"No") << L"\n";
}

bool FPSOverlay::LoadConfigFromArgs(int argc, wchar_t* argv[]) {
    // Configuration loading is handled in ProcessCommandLine
    return true;
}