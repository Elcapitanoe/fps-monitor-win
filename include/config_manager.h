#pragma once

#include "common.h"

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    // Load configuration from file
    bool LoadConfig(const std::wstring& configPath = CONFIG_FILE);
    
    // Save configuration to file
    bool SaveConfig(const std::wstring& configPath = CONFIG_FILE);
    
    // Get current configuration
    const OverlayConfig& GetConfig() const { return m_config; }
    
    // Update configuration
    void UpdateConfig(const OverlayConfig& config);
    
    // Auto-scale font size based on screen resolution
    int GetScaledFontSize() const;
    
    // Get screen resolution
    void GetScreenResolution(int& width, int& height) const;

private:
    OverlayConfig m_config;
    
    // Helper functions for INI file parsing
    std::wstring ReadIniString(const std::wstring& section, const std::wstring& key, 
                              const std::wstring& defaultValue, const std::wstring& filePath);
    int ReadIniInt(const std::wstring& section, const std::wstring& key, 
                   int defaultValue, const std::wstring& filePath);
    bool ReadIniBool(const std::wstring& section, const std::wstring& key, 
                     bool defaultValue, const std::wstring& filePath);
    float ReadIniFloat(const std::wstring& section, const std::wstring& key, 
                       float defaultValue, const std::wstring& filePath);
    
    bool WriteIniString(const std::wstring& section, const std::wstring& key, 
                       const std::wstring& value, const std::wstring& filePath);
    bool WriteIniInt(const std::wstring& section, const std::wstring& key, 
                     int value, const std::wstring& filePath);
    bool WriteIniBool(const std::wstring& section, const std::wstring& key, 
                      bool value, const std::wstring& filePath);
    bool WriteIniFloat(const std::wstring& section, const std::wstring& key, 
                       float value, const std::wstring& filePath);
    
    // Parse color from string (e.g., "255,255,255,255" or "1.0,1.0,1.0,1.0")
    Color ParseColor(const std::wstring& colorStr, const Color& defaultColor);
    std::wstring ColorToString(const Color& color);
};