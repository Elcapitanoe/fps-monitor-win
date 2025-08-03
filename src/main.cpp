#include "fps_overlay.h"
#include "menu_manager.h"
#include "utils.h"
#include <iostream>

// Global state variables
std::atomic<bool> g_running(false);
std::atomic<float> g_currentFPS(0.0f);
std::mutex g_configMutex;

// Global overlay instance
std::unique_ptr<FPSOverlay> g_overlay;

// Global menu manager instance
std::unique_ptr<MenuManager> g_menuManager;

// Console control handler for graceful shutdown
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        if (g_overlay) {
            g_overlay->Stop();
        }
        if (g_menuManager) {
            g_menuManager->ExitApplication();
        }
        g_running = false;
        return TRUE;
    default:
        return FALSE;
    }
}

// Exception filter for crash handling
LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
    Utils::LogError(L"Unhandled exception occurred. Application will terminate.");
    
    if (g_overlay) {
        g_overlay->Stop();
    }
    if (g_menuManager) {
        g_menuManager->ExitApplication();
    }
    
    return EXCEPTION_EXECUTE_HANDLER;
}

int wmain(int argc, wchar_t* argv[]) {
    // Set up exception handling
    SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    
    // Display application info and antivirus warning
    std::wcout << L"FPS Overlay - https://github.com/Elcapitanoe/fps-monitor-win" << std::endl;
    std::wcout << L"Note: This software may trigger antivirus false positives due to frame detection." << std::endl;
    std::wcout << L"It is safe to use - source code is available on GitHub." << std::endl;
    std::wcout << L"========================================" << std::endl;
    
    // Check for menu mode
    bool menuMode = false;
    for (int i = 1; i < argc; i++) {
        std::wstring arg = argv[i];
        if (arg == L"--menu" || arg == L"-m") {
            menuMode = true;
            break;
        }
    }
    
    if (menuMode) {
        // Initialize and run menu system
        g_menuManager = std::make_unique<MenuManager>();
        if (!g_menuManager->Initialize()) {
            std::wcout << L"Failed to initialize menu system." << std::endl;
            return 1;
        }
        
        g_menuManager->RunMenuLoop();
        return 0;
    }
    
    try {
        // Check if another instance is already running
        if (Utils::IsApplicationAlreadyRunning()) {
            std::wcout << L"FPS Overlay is already running." << std::endl;
            return 1;
        }
        
        // Create mutex to prevent multiple instances
        if (!Utils::CreateMutex(L"Global\\FPSOverlayMutex")) {
            std::wcout << L"Failed to create application mutex." << std::endl;
            return 1;
        }
        
        // Check system compatibility
        if (!Utils::IsWindows7OrLater()) {
            std::wcout << L"This application requires Windows 7 or later." << std::endl;
            return 1;
        }
        
        Utils::LogInfo(L"Starting FPS Overlay v1.2.0");
        Utils::LogInfo(L"System: " + Utils::GetWindowsVersion());
        
        // Create overlay instance
        g_overlay = std::make_unique<FPSOverlay>();
        
        // Process command line arguments
        if (!g_overlay->ProcessCommandLine(argc, argv)) {
            return 0; // Help or version was shown, exit normally
        }
        
        // Initialize the overlay
        if (!g_overlay->Initialize()) {
            std::wcout << L"Failed to initialize FPS overlay." << std::endl;
            Utils::LogError(L"Failed to initialize FPS overlay");
            return 1;
        }
        
        // Start the overlay
        if (!g_overlay->Start()) {
            std::wcout << L"Failed to start FPS overlay." << std::endl;
            Utils::LogError(L"Failed to start FPS overlay");
            return 1;
        }
        
        Utils::LogInfo(L"FPS Overlay started successfully");
        std::wcout << L"FPS Overlay is running. Press Ctrl+C to exit." << std::endl;
        
        g_running = true;
        
        // Main application loop
        while (g_running && g_overlay->IsRunning()) {
            Sleep(100); // Sleep for 100ms to reduce CPU usage
            
            // Update the overlay
            g_overlay->Update();
        }
        
        // Cleanup
        Utils::LogInfo(L"Shutting down FPS Overlay");
        g_overlay->Stop();
        g_overlay.reset();
        
        Utils::ReleaseMutex();
        
    } catch (const std::exception& e) {
        std::wcout << L"Exception: " << Utils::Utf8ToWide(e.what()) << std::endl;
        Utils::LogError(L"Exception: " + Utils::Utf8ToWide(e.what()));
        return 1;
    } catch (...) {
        std::wcout << L"Unknown exception occurred." << std::endl;
        Utils::LogError(L"Unknown exception occurred");
        return 1;
    }
    
    return 0;
}
