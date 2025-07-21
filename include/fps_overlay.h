#pragma once

#include "common.h"
#include "config_manager.h"
#include "hook_manager.h"
#include "renderer.h"

class FPSOverlay {
public:
    FPSOverlay();
    ~FPSOverlay();
    
    // Initialize the FPS overlay system
    bool Initialize();
    
    // Start the overlay
    bool Start();
    
    // Stop the overlay
    void Stop();
    
    // Check if overlay is running
    bool IsRunning() const { return m_running; }
    
    // Update FPS calculation
    void UpdateFPS();
    
    // Get current FPS
    float GetCurrentFPS() const { return m_currentFPS; }
    
    // Process command line arguments
    bool ProcessCommandLine(int argc, wchar_t* argv[]);
    
    // Main update loop (called from worker thread)
    void Update();

private:
    bool m_running;
    bool m_initialized;
    float m_currentFPS;
    
    // Component managers
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<HookManager> m_hookManager;
    std::unique_ptr<Renderer> m_renderer;
    
    // Threading
    std::thread m_updateThread;
    std::mutex m_fpsMutex;
    
    // FPS calculation
    std::vector<float> m_frameTimes;
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    size_t m_frameTimeIndex;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
    size_t m_memoryUsage;
    
    // Private methods
    void UpdateWorker();
    void CalculateFPS(float deltaTime);
    void MonitorMemoryUsage();
    bool CheckSystemCompatibility();
    void SetupExceptionHandling();
    
    // Command line processing
    void ShowHelp();
    void ShowVersion();
    bool LoadConfigFromArgs(int argc, wchar_t* argv[]);
};