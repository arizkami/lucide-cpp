#include <iostream>
#include <string>
#include <vector>
#include <fstream>  // Add this for std::ofstream

// Include LucideIconCpp headers with correct paths
#include "lucideicon.hpp"
#include "src/iconwrapper.hpp"
#include "generated/icon_declarations.hpp"  // Fix the path

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

using namespace LucideIcon;

void printPlatformInfo() {
#ifdef _WIN32
    std::cout << "Platform: Windows" << std::endl;
#elif defined(__linux__)
    std::cout << "Platform: Linux" << std::endl;
#elif defined(__APPLE__)
    std::cout << "Platform: macOS" << std::endl;
#else
    std::cout << "Platform: Unknown" << std::endl;
#endif
}

void demonstrateBasicUsage() {
    std::cout << "\n=== Basic Icon Usage ===" << std::endl;
    
    // Create a basic configuration
    IconConfig config;
    config.setSize(24);  // Use the convenience method
    config.strokeWidth = 2;
    config.setColor("#000000");  // Use the convenience method
    
    // Generate some basic icons
    std::vector<std::string> iconNames = {"home", "user", "settings", "heart", "star"};
    
    for (const auto& iconName : iconNames) {
        try {
            std::string svg = IconRegistry::getInstance().generateSVG(iconName, config);
            std::cout << "Generated " << iconName << " icon (" << svg.length() << " chars)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Failed to generate " << iconName << ": " << e.what() << std::endl;
        }
    }
}

void demonstrateIconWrapper() {
    std::cout << "\n=== Icon Wrapper Usage ===" << std::endl;
    
    try {
        // Use the fluent interface
        auto wrapper = IconWrapper("heart")
            .size(32)
            .strokeWidth(2)  // Use int instead of float
            .color("#ff0000")
            .fill("none");
        
        std::string svg = wrapper.render();
        std::cout << "Heart icon with custom styling generated (" << svg.length() << " chars)" << std::endl;
        
        // Demonstrate different sizes
        std::vector<int> sizes = {16, 24, 32, 48, 64};
        for (int size : sizes) {
            auto sizedIcon = IconWrapper("star").size(size).color("#ffd700");
            std::string sizedSvg = sizedIcon.render();
            std::cout << "Star icon " << size << "px: " << sizedSvg.length() << " chars" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error in wrapper demo: " << e.what() << std::endl;
    }
}

void demonstrateIconCollection() {
    std::cout << "\n=== Icon Collection Usage ===" << std::endl;
    
    try {
        // Create a collection of navigation icons
        IconCollection navIcons("navigation");
        navIcons.addIcon("home")
                .addIcon("user")
                .addIcon("settings")
                .addIcon("search")
                .addIcon("menu");
        
        // Apply consistent styling
        IconConfig navConfig;
        navConfig.setSize(20);
        navConfig.strokeWidth = 2;  // Use int instead of float
        navConfig.setColor("#333333");
        
        auto styledIcons = navIcons.renderAll(navConfig);
        std::cout << "Navigation collection rendered " << styledIcons.size() << " icons" << std::endl;
        
        // Fix structured binding - iterate over vector of strings instead
        for (size_t i = 0; i < styledIcons.size(); ++i) {
            std::cout << "  Icon " << i << ": " << styledIcons[i].length() << " chars" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error in collection demo: " << e.what() << std::endl;
    }
}

void demonstrateIconTheme() {
    std::cout << "\n=== Icon Theme Usage ===" << std::endl;
    
    try {
        // Create a dark theme
        IconTheme darkTheme("dark");
        darkTheme.setDefaultSize(24)
                 .setDefaultStrokeWidth(2.0f)
                 .setDefaultColor("#ffffff")
                 .setDefaultFill("none");
        
        // Create a light theme
        IconTheme lightTheme("light");
        lightTheme.setDefaultSize(24)
                  .setDefaultStrokeWidth(1.5f)
                  .setDefaultColor("#000000")
                  .setDefaultFill("none");
        
        std::vector<std::string> testIcons = {"sun", "moon", "star"};
        
        std::cout << "Dark theme:" << std::endl;
        for (const auto& iconName : testIcons) {
            auto config = darkTheme.getConfig();
            std::string svg = IconRegistry::getInstance().generateSVG(iconName, config);
            std::cout << "  " << iconName << ": " << svg.length() << " chars" << std::endl;
        }
        
        std::cout << "Light theme:" << std::endl;
        for (const auto& iconName : testIcons) {
            auto config = lightTheme.getConfig();
            std::string svg = IconRegistry::getInstance().generateSVG(iconName, config);
            std::cout << "  " << iconName << ": " << svg.length() << " chars" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error in theme demo: " << e.what() << std::endl;
    }
}

void saveIconToFile(const std::string& iconName, const std::string& svg) {
    std::string filename = iconName + ".svg";
    
#ifdef _WIN32
    // Windows file handling
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
#else
    // Linux file handling
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    // Set file permissions on Linux
    chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
    
    if (file.is_open()) {
        file << svg;
        file.close();
        std::cout << "Saved " << filename << std::endl;
    } else {
        std::cout << "Failed to save " << filename << std::endl;
    }
}

void demonstrateFileOutput() {
    std::cout << "\n=== File Output Demo ===" << std::endl;
    
    try {
        IconConfig config;
        config.size = 48;
        config.strokeWidth = 2.0f;
        config.color = "#007acc";
        
        // Generate and save a few icons
        std::vector<std::string> iconsToSave = {"download", "upload", "save"};
        
        for (const auto& iconName : iconsToSave) {
            std::string svg = IconRegistry::getInstance().generateSVG(iconName, config);
            saveIconToFile(iconName, svg);
        }
    } catch (const std::exception& e) {
        std::cout << "Error in file output demo: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "LucideIconCpp Example Application" << std::endl;
    std::cout << "=================================" << std::endl;
    
    printPlatformInfo();
    
    try {
        // Get registry info
        auto& registry = IconRegistry::getInstance();
        auto iconNames = registry.getAvailableIcons();
        std::cout << "\nLoaded " << iconNames.size() << " icons from registry" << std::endl;
        
        // Run demonstrations
        demonstrateBasicUsage();
        demonstrateIconWrapper();
        demonstrateIconCollection();
        demonstrateIconTheme();
        demonstrateFileOutput();
        
        std::cout << "\n=== Example completed successfully! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}