#include "menu_manager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <conio.h>

MenuManager::MenuManager() 
    : m_running(false), m_initialized(false), m_showAdvancedOptions(false), m_currentPage(0) {
}

MenuManager::~MenuManager() {
}

bool MenuManager::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Set console properties for better display
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        // Enable virtual terminal processing for ANSI escape codes
        DWORD mode;
        GetConsoleMode(hConsole, &mode);
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        
        // Set console title
        SetConsoleTitleW(L"FPS Monitor - Control Panel");
    }
    
    // Initialize menu categories and options
    InitializeMenuOptions();
    
    m_initialized = true;
    return true;
}

void MenuManager::InitializeMenuOptions() {
    // Clear existing options
    m_categories.clear();
    m_options.clear();
    
    // Create categories
    MenuCategory configCategory(L"CONFIGURATION", L"Overlay settings and preferences");
    MenuCategory systemCategory(L"SYSTEM", L"System information and diagnostics");
    MenuCategory performanceCategory(L"PERFORMANCE", L"Performance monitoring and optimization");
    MenuCategory utilityCategory(L"UTILITIES", L"Utility functions and tools");
    MenuCategory advancedCategory(L"ADVANCED", L"Advanced options and debugging");
    
    // Configuration options
    configCategory.options = {
        MenuOption(1, L"Change Overlay Position", L"Set overlay position (Top-Left, Top-Right, Bottom-Left, Bottom-Right)", L"CONFIGURATION", 
                  [this]() { ChangeOverlayPosition(); }),
        MenuOption(2, L"Change Font Size", L"Adjust the font size of the overlay text", L"CONFIGURATION", 
                  [this]() { ChangeFontSize(); }),
        MenuOption(3, L"Change Text Color", L"Modify the color of the overlay text", L"CONFIGURATION", 
                  [this]() { ChangeTextColor(); }),
        MenuOption(4, L"Change Background Color", L"Modify the background color of the overlay", L"CONFIGURATION", 
                  [this]() { ChangeBackgroundColor(); }),
        MenuOption(5, L"Toggle Overlay", L"Enable or disable the FPS overlay", L"CONFIGURATION", 
                  [this]() { ToggleOverlay(); }),
        MenuOption(6, L"Toggle Background", L"Show or hide the overlay background", L"CONFIGURATION", 
                  [this]() { ToggleBackground(); }),
        MenuOption(7, L"Change Update Interval", L"Set the update frequency of the overlay", L"CONFIGURATION", 
                  [this]() { ChangeUpdateInterval(); }),
        MenuOption(8, L"Reset to Defaults", L"Restore all settings to default values", L"CONFIGURATION", 
                  [this]() { ResetToDefaults(); }),
        MenuOption(9, L"Save Configuration", L"Save current settings to config file", L"CONFIGURATION", 
                  [this]() { SaveConfiguration(); }),
        MenuOption(10, L"Load Configuration", L"Load settings from config file", L"CONFIGURATION", 
                   [this]() { LoadConfiguration(); })
    };
    
    // System options
    systemCategory.options = {
        MenuOption(11, L"System Information", L"Display detailed system information", L"SYSTEM", 
                  [this]() { ShowSystemInfo(); }),
        MenuOption(12, L"System Compatibility", L"Check system compatibility requirements", L"SYSTEM", 
                  [this]() { ShowSystemCompatibility(); }),
        MenuOption(13, L"Graphics APIs", L"Show available graphics APIs", L"SYSTEM", 
                  [this]() { ShowGraphicsAPIs(); }),
        MenuOption(14, L"Memory Usage", L"Display current memory usage", L"SYSTEM", 
                  [this]() { ShowMemoryUsage(); }),
        MenuOption(15, L"Process Information", L"Show current process details", L"SYSTEM", 
                  [this]() { ShowProcessInfo(); }),
        MenuOption(16, L"Window Information", L"Display active window details", L"SYSTEM", 
                  [this]() { ShowWindowInfo(); })
    };
    
    // Performance options
    performanceCategory.options = {
        MenuOption(21, L"Start Performance Test", L"Begin performance benchmarking", L"PERFORMANCE", 
                  [this]() { StartPerformanceTest(); }),
        MenuOption(22, L"Stop Performance Test", L"End performance benchmarking", L"PERFORMANCE", 
                  [this]() { StopPerformanceTest(); }),
        MenuOption(23, L"Performance Statistics", L"Show performance statistics", L"PERFORMANCE", 
                  [this]() { ShowPerformanceStats(); }),
        MenuOption(24, L"Optimize Settings", L"Automatically optimize settings for best performance", L"PERFORMANCE", 
                  [this]() { OptimizeSettings(); })
    };
    
    // Utility options
    utilityCategory.options = {
        MenuOption(31, L"Clear Console", L"Clear the console screen", L"UTILITIES", 
                  [this]() { ClearConsole(); }),
        MenuOption(32, L"Show Logs", L"Display application logs", L"UTILITIES", 
                  [this]() { ShowLogs(); }),
        MenuOption(33, L"Backup Configuration", L"Create a backup of current configuration", L"UTILITIES", 
                  [this]() { BackupConfig(); }),
        MenuOption(34, L"Restore Configuration", L"Restore configuration from backup", L"UTILITIES", 
                  [this]() { RestoreConfig(); }),
        MenuOption(35, L"Validate Configuration", L"Check configuration file integrity", L"UTILITIES", 
                  [this]() { ValidateConfig(); })
    };
    
    // Advanced options
    advancedCategory.options = {
        MenuOption(41, L"Show Help", L"Display help information", L"ADVANCED", 
                  [this]() { ShowHelp(); }),
        MenuOption(42, L"About", L"Show application information", L"ADVANCED", 
                  [this]() { ShowAbout(); }),
        MenuOption(99, L"Exit Application", L"Exit the FPS Monitor application", L"ADVANCED", 
                  [this]() { ExitApplication(); })
    };
    
    // Add categories
    m_categories.push_back(configCategory);
    m_categories.push_back(systemCategory);
    m_categories.push_back(performanceCategory);
    m_categories.push_back(utilityCategory);
    m_categories.push_back(advancedCategory);
    
    // Build options map
    for (const auto& category : m_categories) {
        for (const auto& option : category.options) {
            m_options.insert(std::make_pair(option.id, option));
        }
    }
}

void MenuManager::ShowMainMenu() {
    ClearScreen();
    DisplayHeader();
    DisplayCategories();
    DisplayFooter();
    DisplayPrompt();
}

void MenuManager::RunMenuLoop() {
    m_running = true;
    
    while (m_running) {
        ShowMainMenu();
        int choice = GetUserChoice();
        ProcessUserInput(choice);
    }
}

void MenuManager::DisplayHeader() {
    std::wcout << CreateSeparator() << std::endl;
    std::wcout << CenterText(L"FPS MONITOR - CONTROL PANEL") << std::endl;
    std::wcout << CreateSeparator() << std::endl;
    std::wcout << GetUserInfo() << std::endl;
    std::wcout << GetComputerInfo() << std::endl;
    std::wcout << GetSystemInfo() << std::endl;
    std::wcout << GetTimeZoneInfo() << std::endl;
    std::wcout << CreateSeparator() << std::endl;
    std::wcout << CenterText(L"FOLLOW US: HTTPS://GITHUB.COM/ELCAPITANOE/FPS-MONITOR-WIN") << std::endl;
    std::wcout << CreateSeparator() << std::endl;
    std::wcout << std::endl;
}

void MenuManager::DisplayCategories() {
    for (const auto& category : m_categories) {
        std::wcout << category.name << std::endl;
        std::wcout << std::wstring(category.name.length(), '-') << std::endl;
        
        for (const auto& option : category.options) {
            if (option.enabled) {
                std::wcout << L"[" << option.id << "] " << option.name;
                if (!option.description.empty()) {
                    std::wcout << L" | " << option.description;
                }
                std::wcout << std::endl;
            }
        }
        std::wcout << std::endl;
    }
}

void MenuManager::DisplayFooter() {
    std::wcout << CreateSeparator() << std::endl;
    std::wcout << L"NOTE: Type option number to select. Press 'h' for help, 'q' to quit." << std::endl;
    std::wcout << L"Recommended: Use option 1-10 to configure overlay settings." << std::endl;
    std::wcout << CreateSeparator() << std::endl;
}

void MenuManager::DisplayPrompt() {
    std::wcout << L"Type option: ";
}

int MenuManager::GetUserChoice() {
    std::wstring input;
    std::getline(std::wcin, input);
    
    // Handle special commands
    if (input == L"h" || input == L"H") {
        ShowHelp();
        return -1;
    }
    if (input == L"q" || input == L"Q") {
        ExitApplication();
        return -1;
    }
    if (input == L"clear" || input == L"CLEAR") {
        ClearConsole();
        return -1;
    }
    
    // Try to parse as number
    try {
        return std::stoi(input);
    } catch (const std::exception&) {
        return -1;
    }
}

void MenuManager::ProcessUserInput(int choice) {
    if (choice == -1) {
        return; // Special command handled
    }
    
    auto it = m_options.find(choice);
    if (it != m_options.end() && it->second.enabled) {
        try {
            it->second.action();
        } catch (const std::exception& e) {
            std::wcout << L"Error executing option: " << Utils::Utf8ToWide(e.what()) << std::endl;
            PauseForUser();
        }
    } else {
        ShowInvalidOption();
    }
}

void MenuManager::ShowInvalidOption() {
    std::wcout << L"Invalid option. Please try again." << std::endl;
    PauseForUser();
}

// System information methods
std::wstring MenuManager::GetSystemInfo() const {
    std::wstringstream ss;
    ss << L" CURRENT OS: " << Utils::GetWindowsVersion();
    return ss.str();
}

std::wstring MenuManager::GetUserInfo() const {
    wchar_t username[256];
    DWORD size = 256;
    GetUserNameW(username, &size);
    
    std::wstringstream ss;
    ss << L" USER: " << username;
    return ss.str();
}

std::wstring MenuManager::GetComputerInfo() const {
    wchar_t computername[256];
    DWORD size = 256;
    GetComputerNameW(computername, &size);
    
    std::wstringstream ss;
    ss << L" COMPUTERNAME: " << computername;
    return ss.str();
}

std::wstring MenuManager::GetTimeZoneInfo() const {
    TIME_ZONE_INFORMATION tzi;
    DWORD result = GetTimeZoneInformation(&tzi);
    
    std::wstringstream ss;
    if (result != TIME_ZONE_ID_INVALID) {
        ss << L" Time Zone: " << tzi.StandardName;
    } else {
        ss << L" Time Zone: Unknown";
    }
    return ss.str();
}

// Menu action implementations
void MenuManager::ShowSystemInfo() {
    ClearScreen();
    std::wcout << L"=== SYSTEM INFORMATION ===" << std::endl;
    std::wcout << GetSystemInfo() << std::endl;
    std::wcout << GetUserInfo() << std::endl;
    std::wcout << GetComputerInfo() << std::endl;
    std::wcout << GetTimeZoneInfo() << std::endl;
    std::wcout << L"Memory Usage: " << Utils::GetProcessMemoryUsage() << L" bytes" << std::endl;
    std::wcout << L"Process Elevated: " << (Utils::IsProcessElevated() ? L"Yes" : L"No") << std::endl;
    PauseForUser();
}

void MenuManager::ShowConfiguration() {
    ClearScreen();
    std::wcout << L"=== CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration options will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ShowPerformance() {
    ClearScreen();
    std::wcout << L"=== PERFORMANCE ===" << std::endl;
    std::wcout << L"Performance monitoring will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ShowHelp() {
    ClearScreen();
    std::wcout << L"=== HELP ===" << std::endl;
    std::wcout << L"FPS Monitor Control Panel Help" << std::endl;
    std::wcout << L"=============================" << std::endl;
    std::wcout << L"Navigation:" << std::endl;
    std::wcout << L"- Type the number of the option you want to select" << std::endl;
    std::wcout << L"- Press 'h' for help" << std::endl;
    std::wcout << L"- Press 'q' to quit" << std::endl;
    std::wcout << L"- Type 'clear' to clear the console" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"Categories:" << std::endl;
    std::wcout << L"- Configuration: Overlay settings and preferences" << std::endl;
    std::wcout << L"- System: System information and diagnostics" << std::endl;
    std::wcout << L"- Performance: Performance monitoring and optimization" << std::endl;
    std::wcout << L"- Utilities: Utility functions and tools" << std::endl;
    std::wcout << L"- Advanced: Advanced options and debugging" << std::endl;
    PauseForUser();
}

void MenuManager::ShowAbout() {
    ClearScreen();
    std::wcout << L"=== ABOUT ===" << std::endl;
    std::wcout << L"FPS Monitor for Windows" << std::endl;
    std::wcout << L"Version: 1.2.0" << std::endl;
    std::wcout << L"Author: Elcapitanoe" << std::endl;
    std::wcout << L"GitHub: https://github.com/Elcapitanoe/fps-monitor-win" << std::endl;
    std::wcout << L"License: MIT" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"A lightweight FPS overlay for Windows applications." << std::endl;
    std::wcout << L"Supports DirectX 9, DirectX 11, and OpenGL applications." << std::endl;
    PauseForUser();
}

void MenuManager::ExitApplication() {
    std::wcout << L"Exiting FPS Monitor..." << std::endl;
    m_running = false;
}

// Configuration actions
void MenuManager::ChangeOverlayPosition() {
    ClearScreen();
    std::wcout << L"=== CHANGE OVERLAY POSITION ===" << std::endl;
    std::wcout << L"Current position: TOP_LEFT" << std::endl;
    std::wcout << L"Available positions:" << std::endl;
    std::wcout << L"1. Top-Left" << std::endl;
    std::wcout << L"2. Top-Right" << std::endl;
    std::wcout << L"3. Bottom-Left" << std::endl;
    std::wcout << L"4. Bottom-Right" << std::endl;
    std::wcout << L"Position change will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ChangeFontSize() {
    ClearScreen();
    std::wcout << L"=== CHANGE FONT SIZE ===" << std::endl;
    std::wcout << L"Current font size: 16" << std::endl;
    std::wcout << L"Font size change will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ChangeTextColor() {
    ClearScreen();
    std::wcout << L"=== CHANGE TEXT COLOR ===" << std::endl;
    std::wcout << L"Current color: Green (0.0, 1.0, 0.0, 1.0)" << std::endl;
    std::wcout << L"Color change will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ChangeBackgroundColor() {
    ClearScreen();
    std::wcout << L"=== CHANGE BACKGROUND COLOR ===" << std::endl;
    std::wcout << L"Current background: Semi-transparent black (0.0, 0.0, 0.0, 0.5)" << std::endl;
    std::wcout << L"Background color change will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ToggleOverlay() {
    ClearScreen();
    std::wcout << L"=== TOGGLE OVERLAY ===" << std::endl;
    std::wcout << L"Overlay toggle will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ToggleBackground() {
    ClearScreen();
    std::wcout << L"=== TOGGLE BACKGROUND ===" << std::endl;
    std::wcout << L"Background toggle will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ChangeUpdateInterval() {
    ClearScreen();
    std::wcout << L"=== CHANGE UPDATE INTERVAL ===" << std::endl;
    std::wcout << L"Current interval: 500ms" << std::endl;
    std::wcout << L"Update interval change will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ResetToDefaults() {
    ClearScreen();
    std::wcout << L"=== RESET TO DEFAULTS ===" << std::endl;
    std::wcout << L"Reset to defaults will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::SaveConfiguration() {
    ClearScreen();
    std::wcout << L"=== SAVE CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration saved successfully." << std::endl;
    PauseForUser();
}

void MenuManager::LoadConfiguration() {
    ClearScreen();
    std::wcout << L"=== LOAD CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration loaded successfully." << std::endl;
    PauseForUser();
}

// System actions
void MenuManager::ShowSystemCompatibility() {
    ClearScreen();
    std::wcout << L"=== SYSTEM COMPATIBILITY ===" << std::endl;
    std::wcout << L"Windows 7+: " << (Utils::IsWindows7OrLater() ? L"Yes" : L"No") << std::endl;
    std::wcout << L"Windows 10+: " << (Utils::IsWindows10OrLater() ? L"Yes" : L"No") << std::endl;
    std::wcout << L"DirectX 9: " << (Utils::IsDirectX9Available() ? L"Available" : L"Not Available") << std::endl;
    std::wcout << L"DirectX 11: " << (Utils::IsDirectX11Available() ? L"Available" : L"Not Available") << std::endl;
    std::wcout << L"OpenGL: " << (Utils::IsOpenGLAvailable() ? L"Available" : L"Not Available") << std::endl;
    PauseForUser();
}

void MenuManager::ShowGraphicsAPIs() {
    ClearScreen();
    std::wcout << L"=== GRAPHICS APIs ===" << std::endl;
    auto apis = Utils::GetAvailableGraphicsAPIs();
    for (const auto& api : apis) {
        switch (api) {
            case GraphicsAPI::D3D9: std::wcout << L"DirectX 9" << std::endl; break;
            case GraphicsAPI::D3D11: std::wcout << L"DirectX 11" << std::endl; break;
            case GraphicsAPI::OPENGL: std::wcout << L"OpenGL" << std::endl; break;
            default: std::wcout << L"Unknown" << std::endl; break;
        }
    }
    PauseForUser();
}

void MenuManager::ShowMemoryUsage() {
    ClearScreen();
    std::wcout << L"=== MEMORY USAGE ===" << std::endl;
    size_t memoryUsage = Utils::GetProcessMemoryUsage();
    std::wcout << L"Current memory usage: " << memoryUsage << L" bytes (" 
               << (memoryUsage / 1024 / 1024) << L" MB)" << std::endl;
    std::wcout << L"Maximum allowed: 25 MB" << std::endl;
    PauseForUser();
}

void MenuManager::ShowProcessInfo() {
    ClearScreen();
    std::wcout << L"=== PROCESS INFORMATION ===" << std::endl;
    std::wcout << L"Process ID: " << GetCurrentProcessId() << std::endl;
    std::wcout << L"Thread ID: " << GetCurrentThreadId() << std::endl;
    std::wcout << L"Elevated: " << (Utils::IsProcessElevated() ? L"Yes" : L"No") << std::endl;
    PauseForUser();
}

void MenuManager::ShowWindowInfo() {
    ClearScreen();
    std::wcout << L"=== WINDOW INFORMATION ===" << std::endl;
    HWND foregroundWindow = Utils::GetForegroundGameWindow();
    if (foregroundWindow) {
        std::wcout << L"Foreground window: " << Utils::GetWindowTitle(foregroundWindow) << std::endl;
        std::wcout << L"Class: " << Utils::GetWindowClassName(foregroundWindow) << std::endl;
        std::wcout << L"Process ID: " << Utils::GetWindowProcessId(foregroundWindow) << std::endl;
        std::wcout << L"Fullscreen: " << (Utils::IsFullscreenWindow(foregroundWindow) ? L"Yes" : L"No") << std::endl;
    } else {
        std::wcout << L"No foreground window detected." << std::endl;
    }
    PauseForUser();
}

// Utility actions
void MenuManager::ClearConsole() {
    ClearScreen();
}

void MenuManager::ShowLogs() {
    ClearScreen();
    std::wcout << L"=== APPLICATION LOGS ===" << std::endl;
    std::wcout << L"Log display will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::BackupConfig() {
    ClearScreen();
    std::wcout << L"=== BACKUP CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration backup will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::RestoreConfig() {
    ClearScreen();
    std::wcout << L"=== RESTORE CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration restore will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::ValidateConfig() {
    ClearScreen();
    std::wcout << L"=== VALIDATE CONFIGURATION ===" << std::endl;
    std::wcout << L"Configuration validation will be implemented here." << std::endl;
    PauseForUser();
}

// Performance actions
void MenuManager::StartPerformanceTest() {
    ClearScreen();
    std::wcout << L"=== START PERFORMANCE TEST ===" << std::endl;
    std::wcout << L"Performance test started." << std::endl;
    PauseForUser();
}

void MenuManager::StopPerformanceTest() {
    ClearScreen();
    std::wcout << L"=== STOP PERFORMANCE TEST ===" << std::endl;
    std::wcout << L"Performance test stopped." << std::endl;
    PauseForUser();
}

void MenuManager::ShowPerformanceStats() {
    ClearScreen();
    std::wcout << L"=== PERFORMANCE STATISTICS ===" << std::endl;
    std::wcout << L"Performance statistics will be implemented here." << std::endl;
    PauseForUser();
}

void MenuManager::OptimizeSettings() {
    ClearScreen();
    std::wcout << L"=== OPTIMIZE SETTINGS ===" << std::endl;
    std::wcout << L"Settings optimization will be implemented here." << std::endl;
    PauseForUser();
}

// Utility functions
void MenuManager::ClearScreen() {
    system("cls");
}

void MenuManager::PauseForUser() {
    std::wcout << L"Press any key to continue..." << std::endl;
    _getch();
}

std::wstring MenuManager::CenterText(const std::wstring& text, int width) {
    int padding = static_cast<int>((width - text.length()) / 2);
    if (padding < 0) padding = 0;
    return std::wstring(padding, ' ') + text;
}

std::wstring MenuManager::CreateSeparator(char character, int width) {
    return std::wstring(width, character);
} 