#include "config_manager.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include <iomanip>

ConfigManager::ConfigManager() {
    // Initialize default configuration
    m_config = OverlayConfig();
}

ConfigManager::~ConfigManager() {
    // Save configuration on destruction
    SaveConfig();
}

bool ConfigManager::LoadConfig(const std::wstring& configPath) {
    std::wstring fullPath = Utils::GetExecutableDirectory() + L"\\" + configPath;
    
    if (!Utils::FileExists(fullPath)) {
        Utils::LogInfo(L"Config file not found, creating default: " + fullPath);
        return SaveConfig(configPath); // Create default config
    }
    
    try {
        // Load general settings
        m_config.enabled = ReadIniBool(L"General", L"Enabled", true, fullPath);
        m_config.updateInterval = ReadIniInt(L"General", L"UpdateInterval", DEFAULT_UPDATE_INTERVAL, fullPath);
        
        // Load appearance settings
        int position = ReadIniInt(L"Appearance", L"Position", static_cast<int>(OverlayPosition::TOP_LEFT), fullPath);
        m_config.position = static_cast<OverlayPosition>(position);
        
        m_config.fontSize = ReadIniInt(L"Appearance", L"FontSize", 0, fullPath); // 0 = auto-scale
        m_config.fontName = ReadIniString(L"Appearance", L"FontName", L"Consolas", fullPath);
        m_config.offsetX = ReadIniInt(L"Appearance", L"OffsetX", 10, fullPath);
        m_config.offsetY = ReadIniInt(L"Appearance", L"OffsetY", 10, fullPath);
        m_config.showBackground = ReadIniBool(L"Appearance", L"ShowBackground", true, fullPath);
        
        // Load colors
        std::wstring textColorStr = ReadIniString(L"Colors", L"TextColor", L"0.0,1.0,0.0,1.0", fullPath);
        m_config.textColor = ParseColor(textColorStr, Color(0.0f, 1.0f, 0.0f, 1.0f));
        
        std::wstring bgColorStr = ReadIniString(L"Colors", L"BackgroundColor", L"0.0,0.0,0.0,0.5", fullPath);
        m_config.backgroundColor = ParseColor(bgColorStr, Color(0.0f, 0.0f, 0.0f, 0.5f));
        
        // Auto-scale font size if set to 0
        if (m_config.fontSize <= 0) {
            m_config.fontSize = GetScaledFontSize();
        }
        
        // Validate font availability
        if (!Utils::IsFontInstalled(m_config.fontName)) {
            std::vector<std::wstring> preferredFonts = {L"Consolas", L"Courier New", L"Arial"};
            m_config.fontName = Utils::GetBestAvailableFont(preferredFonts);
            Utils::LogWarning(L"Font not found, using fallback: " + m_config.fontName);
        }
        
        Utils::LogInfo(L"Configuration loaded successfully from: " + fullPath);
        return true;
        
    } catch (...) {
        Utils::LogError(L"Failed to load configuration from: " + fullPath);
        return false;
    }
}

bool ConfigManager::SaveConfig(const std::wstring& configPath) {
    std::wstring fullPath = Utils::GetExecutableDirectory() + L"\\" + configPath;
    
    try {
        // Create directory if it doesn't exist
        size_t lastSlash = fullPath.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            std::wstring dir = fullPath.substr(0, lastSlash);
            Utils::CreateDirectoryRecursive(dir);
        }
        
        // Save general settings
        WriteIniBool(L"General", L"Enabled", m_config.enabled, fullPath);
        WriteIniInt(L"General", L"UpdateInterval", m_config.updateInterval, fullPath);
        
        // Save appearance settings
        WriteIniInt(L"Appearance", L"Position", static_cast<int>(m_config.position), fullPath);
        WriteIniInt(L"Appearance", L"FontSize", m_config.fontSize, fullPath);
        WriteIniString(L"Appearance", L"FontName", m_config.fontName, fullPath);
        WriteIniInt(L"Appearance", L"OffsetX", m_config.offsetX, fullPath);
        WriteIniInt(L"Appearance", L"OffsetY", m_config.offsetY, fullPath);
        WriteIniBool(L"Appearance", L"ShowBackground", m_config.showBackground, fullPath);
        
        // Save colors
        WriteIniString(L"Colors", L"TextColor", ColorToString(m_config.textColor), fullPath);
        WriteIniString(L"Colors", L"BackgroundColor", ColorToString(m_config.backgroundColor), fullPath);
        
        // Write configuration comments
        std::wofstream file(fullPath, std::ios::app);
        if (file.is_open()) {
            file << L"\n; FPS Overlay Configuration File\n";
            file << L"; Position: 0=Top-Left, 1=Top-Right, 2=Bottom-Left, 3=Bottom-Right\n";
            file << L"; FontSize: 0=Auto-scale based on resolution, or specify custom size\n";
            file << L"; Colors: R,G,B,A values (0.0-1.0 range)\n";
            file << L"; UpdateInterval: Milliseconds between FPS updates (recommended: 500-1000)\n";
            file.close();
        }
        
        Utils::LogInfo(L"Configuration saved successfully to: " + fullPath);
        return true;
        
    } catch (...) {
        Utils::LogError(L"Failed to save configuration to: " + fullPath);
        return false;
    }
}

void ConfigManager::UpdateConfig(const OverlayConfig& config) {
    std::lock_guard<std::mutex> lock(g_configMutex);
    m_config = config;
}

int ConfigManager::GetScaledFontSize() const {
    int screenWidth, screenHeight;
    GetScreenResolution(screenWidth, screenHeight);
    
    // Base font size for 1920x1080
    const int baseFontSize = 16;
    const int baseWidth = 1920;
    
    // Scale font size based on horizontal resolution
    int scaledSize = (baseFontSize * screenWidth) / baseWidth;
    
    // Clamp to reasonable range
    scaledSize = std::max(12, std::min(scaledSize, 32));
    
    return scaledSize;
}

void ConfigManager::GetScreenResolution(int& width, int& height) const {
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
}

// INI file helper functions implementation
std::wstring ConfigManager::ReadIniString(const std::wstring& section, const std::wstring& key,
                                         const std::wstring& defaultValue, const std::wstring& filePath) {
    wchar_t buffer[1024];
    DWORD result = GetPrivateProfileStringW(section.c_str(), key.c_str(), defaultValue.c_str(),
                                           buffer, sizeof(buffer) / sizeof(wchar_t), filePath.c_str());
    return std::wstring(buffer);
}

int ConfigManager::ReadIniInt(const std::wstring& section, const std::wstring& key,
                             int defaultValue, const std::wstring& filePath) {
    return GetPrivateProfileIntW(section.c_str(), key.c_str(), defaultValue, filePath.c_str());
}

bool ConfigManager::ReadIniBool(const std::wstring& section, const std::wstring& key,
                               bool defaultValue, const std::wstring& filePath) {
    int value = ReadIniInt(section, key, defaultValue ? 1 : 0, filePath);
    return value != 0;
}

float ConfigManager::ReadIniFloat(const std::wstring& section, const std::wstring& key,
                                 float defaultValue, const std::wstring& filePath) {
    std::wstring valueStr = ReadIniString(section, key, L"", filePath);
    if (valueStr.empty()) return defaultValue;
    
    try {
        return std::stof(valueStr);
    } catch (...) {
        return defaultValue;
    }
}

bool ConfigManager::WriteIniString(const std::wstring& section, const std::wstring& key,
                                  const std::wstring& value, const std::wstring& filePath) {
    return WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), filePath.c_str()) != 0;
}

bool ConfigManager::WriteIniInt(const std::wstring& section, const std::wstring& key,
                                int value, const std::wstring& filePath) {
    std::wstring valueStr = std::to_wstring(value);
    return WriteIniString(section, key, valueStr, filePath);
}

bool ConfigManager::WriteIniBool(const std::wstring& section, const std::wstring& key,
                                 bool value, const std::wstring& filePath) {
    return WriteIniInt(section, key, value ? 1 : 0, filePath);
}

bool ConfigManager::WriteIniFloat(const std::wstring& section, const std::wstring& key,
                                  float value, const std::wstring& filePath) {
    std::wostringstream oss;
    oss << std::fixed << std::setprecision(3) << value;
    return WriteIniString(section, key, oss.str(), filePath);
}

Color ConfigManager::ParseColor(const std::wstring& colorStr, const Color& defaultColor) {
    if (colorStr.empty()) return defaultColor;
    
    try {
        std::wistringstream iss(colorStr);
        std::wstring token;
        std::vector<float> components;
        
        while (std::getline(iss, token, L',') && components.size() < 4) {
            components.push_back(std::stof(Utils::Trim(token)));
        }
        
        if (components.size() >= 3) {
            Color color;
            color.r = std::max(0.0f, std::min(1.0f, components[0]));
            color.g = std::max(0.0f, std::min(1.0f, components[1]));
            color.b = std::max(0.0f, std::min(1.0f, components[2]));
            color.a = components.size() >= 4 ? std::max(0.0f, std::min(1.0f, components[3])) : 1.0f;
            return color;
        }
        
    } catch (...) {
        Utils::LogWarning(L"Failed to parse color string: " + colorStr);
    }
    
    return defaultColor;
}

std::wstring ConfigManager::ColorToString(const Color& color) {
    std::wostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << color.r << L"," << color.g << L"," << color.b << L"," << color.a;
    return oss.str();
}