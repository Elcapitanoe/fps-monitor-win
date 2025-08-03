#pragma once

#include "common.h"
#include "config_manager.h"
#include <functional>
#include <map>
#include <vector>

// Menu option structure
struct MenuOption {
    int id;
    std::wstring name;
    std::wstring description;
    std::wstring category;
    std::function<void()> action;
    bool enabled;
    
    MenuOption(int optionId, const std::wstring& optionName, 
               const std::wstring& optionDescription, const std::wstring& optionCategory,
               std::function<void()> optionAction, bool optionEnabled = true)
        : id(optionId), name(optionName), description(optionDescription), 
          category(optionCategory), action(optionAction), enabled(optionEnabled) {}
};

// Menu category structure
struct MenuCategory {
    std::wstring name;
    std::wstring description;
    std::vector<MenuOption> options;
    
    MenuCategory(const std::wstring& categoryName, const std::wstring& categoryDescription = L"")
        : name(categoryName), description(categoryDescription) {}
};

class MenuManager {
public:
    MenuManager();
    ~MenuManager();
    
    // Initialize the menu system
    bool Initialize();
    
    // Display the main menu
    void ShowMainMenu();
    
    // Run the menu loop
    void RunMenuLoop();
    
    // Add menu options
    void AddOption(const MenuOption& option);
    void AddCategory(const MenuCategory& category);
    
    // Menu actions
    void ShowSystemInfo();
    void ShowConfiguration();
    void ShowPerformance();
    void ShowHelp();
    void ShowAbout();
    void ExitApplication();
    
    // Configuration actions
    void ChangeOverlayPosition();
    void ChangeFontSize();
    void ChangeTextColor();
    void ChangeBackgroundColor();
    void ToggleOverlay();
    void ToggleBackground();
    void ChangeUpdateInterval();
    void ResetToDefaults();
    void SaveConfiguration();
    void LoadConfiguration();
    
    // System actions
    void ShowSystemCompatibility();
    void ShowGraphicsAPIs();
    void ShowMemoryUsage();
    void ShowProcessInfo();
    void ShowWindowInfo();
    
    // Utility actions
    void ClearConsole();
    void ShowLogs();
    void BackupConfig();
    void RestoreConfig();
    void ValidateConfig();
    
    // Performance actions
    void StartPerformanceTest();
    void StopPerformanceTest();
    void ShowPerformanceStats();
    void OptimizeSettings();
    
    // Get system information
    std::wstring GetSystemInfo() const;
    std::wstring GetUserInfo() const;
    std::wstring GetComputerInfo() const;
    std::wstring GetTimeZoneInfo() const;
    
private:
    std::vector<MenuCategory> m_categories;
    std::map<int, MenuOption> m_options;
    bool m_running;
    bool m_initialized;
    
    // Menu display helpers
    void DisplayHeader();
    void DisplayCategories();
    void DisplayFooter();
    void DisplayPrompt();
    
    // Input handling
    int GetUserChoice();
    void ProcessUserInput(int choice);
    void ShowInvalidOption();
    
    // Utility functions
    void ClearScreen();
    void PauseForUser();
    std::wstring CenterText(const std::wstring& text, int width = 80);
    std::wstring CreateSeparator(char character = '=', int width = 80);
    
    // Menu state
    bool m_showAdvancedOptions;
    int m_currentPage;
    static const int OPTIONS_PER_PAGE = 20;
    
    // Private helper to initialize menu options
    void InitializeMenuOptions();
}; 