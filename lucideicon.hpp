#ifndef LUCIDE_ICON_HPP
#define LUCIDE_ICON_HPP

#include <string>
#include <unordered_map>
#include <memory>

namespace LucideIcon {

    // Icon configuration structure
    struct IconConfig {
        int width = 24;
        int height = 24;
        int size = 24;  // Add this for convenience
        std::string stroke = "currentColor";
        int strokeWidth = 2;
        std::string strokeLinecap = "round";
        std::string strokeLinejoin = "round";
        std::string fill = "none";
        std::string color = "currentColor";  // Add this for convenience
        std::string className = "";
        std::string style = "";
        
        // Convenience method to set both width and height
        void setSize(int s) {
            size = s;
            width = s;
            height = s;
        }
        
        // Convenience method to set stroke color
        void setColor(const std::string& c) {
            color = c;
            stroke = c;
        }
    };

    // Base icon class
    class Icon {
    public:
        Icon(const std::string& name, const std::string& pathData);
        virtual ~Icon() = default;

        // Generate SVG string with default config
        std::string toSVG() const;
        
        // Generate SVG string with custom config
        std::string toSVG(const IconConfig& config) const;
        
        // Get icon name
        const std::string& getName() const { return name_; }
        
        // Get raw path data
        const std::string& getPathData() const { return pathData_; }

    private:
        std::string name_;
        std::string pathData_;
        
        std::string buildSVG(const IconConfig& config) const;
    };

    // Icon registry for managing all icons
    class IconRegistry {
    public:
        static IconRegistry& getInstance();
        
        // Register an icon
        void registerIcon(const std::string& name, const std::string& pathData);
        
        // Get icon by name
        std::shared_ptr<Icon> getIcon(const std::string& name) const;
        
        // Check if icon exists
        bool hasIcon(const std::string& name) const;
        
        // Get all icon names
        std::vector<std::string> getAllIconNames() const;
        
        // Get available icons (alias for getAllIconNames)
        std::vector<std::string> getAvailableIcons() const { return getAllIconNames(); }  // Add this method
        
        // Generate SVG directly by name
        std::string generateSVG(const std::string& name, const IconConfig& config = IconConfig{}) const;

    private:
        IconRegistry() = default;
        std::unordered_map<std::string, std::shared_ptr<Icon>> icons_;
    };

    // Utility functions
    namespace Utils {
        // Load icons from directory
        bool loadIconsFromDirectory(const std::string& directory);
        
        // Parse SVG file to extract path data
        std::string extractPathDataFromSVG(const std::string& svgContent);
        
        // Validate icon name
        bool isValidIconName(const std::string& name);
    }

    // Predefined icon functions (auto-generated)
    namespace Icons {
        // Include all generated icon function declarations
        #include "build/generated/icon_declarations.hpp"
    }
}

#endif // LUCIDE_ICON_HPP