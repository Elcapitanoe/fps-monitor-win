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
    
    // Create categories - only keep CONFIGURATION
    MenuCategory configCategory(L"CONFIGURATION", L"Overlay settings and preferences");
    
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
                   [this]() { LoadConfiguration(); }),
        MenuOption(99, L"Exit Application", L"Exit the FPS Monitor application", L"CONFIGURATION", 
                  [this]() { ExitApplication(); })
    };
    
    // Add categories - only CONFIGURATION
    m_categories.push_back(configCategory);
    
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
    std::wcout << L"Use options 1-10 to configure overlay settings. Option 99 to exit." << std::endl;
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
    PauseForUser();
}

void MenuManager::ShowAbout() {
    ClearScreen();
    std::wcout << L"=== ABOUT ===" << std::endl;
    std::wcout << L"FPS Monitor for Windows" << std::endl;
    std::wcout << L"Version: 1.3.0" << std::endl;
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