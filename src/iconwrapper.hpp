#ifndef ICON_WRAPPER_HPP
#define ICON_WRAPPER_HPP

#include "../lucideicon.hpp"
#include <vector>
#include <functional>

namespace LucideIcon {

    // Advanced wrapper for icon manipulation
    class IconWrapper {
    public:
        IconWrapper(const std::string& iconName);
        IconWrapper(std::shared_ptr<Icon> icon);
        
        // Fluent interface for configuration
        IconWrapper& size(int width, int height);
        IconWrapper& size(int size); // Square size
        IconWrapper& stroke(const std::string& color);
        IconWrapper& color(const std::string& color); // Add this method
        IconWrapper& strokeWidth(int width);
        IconWrapper& fill(const std::string& color);
        IconWrapper& className(const std::string& cls);
        IconWrapper& style(const std::string& styleStr);
        
        // Generate final SVG
        std::string render() const;
        
        // Get current configuration
        const IconConfig& getConfig() const { return config_; }
        
        // Reset to default configuration
        IconWrapper& reset();
        
        // Clone wrapper with same configuration
        IconWrapper clone() const;
        
        // Batch operations
        static std::vector<std::string> renderMultiple(
            const std::vector<std::string>& iconNames,
            const IconConfig& config = IconConfig{}
        );
        
    private:
        std::shared_ptr<Icon> icon_;
        IconConfig config_;
    };

    // Icon collection for managing groups of icons
    class IconCollection {
    public:
        IconCollection(const std::string& name);
        
        // Add icons to collection
        void addIcon(const std::string& iconName);
        void addIcons(const std::vector<std::string>& iconNames);
        
        // Remove icons from collection
        void removeIcon(const std::string& iconName);
        void clear();
        
        // Render all icons in collection
        std::vector<std::string> renderAll(const IconConfig& config = IconConfig{}) const;
        
        // Get collection info
        const std::string& getName() const { return name_; }
        size_t size() const { return iconNames_.size(); }
        const std::vector<std::string>& getIconNames() const { return iconNames_; }
        
    private:
        std::string name_;
        std::vector<std::string> iconNames_;
    };

    // Theme support
    class IconTheme {
    public:
        IconTheme(const std::string& name);
        
        // Set theme properties - return references for chaining
        IconTheme& setDefaultStroke(const std::string& color);
        IconTheme& setDefaultFill(const std::string& color);
        IconTheme& setDefaultStrokeWidth(int width);
        IconTheme& setDefaultSize(int size);
        IconTheme& setDefaultColor(const std::string& color);  // Add this method
        
        // Get current theme configuration
        const IconConfig& getConfig() const { return themeConfig_; }  // Add this method
        
        // Apply theme to config
        IconConfig applyTheme(const IconConfig& baseConfig = IconConfig{}) const;
        
        // Predefined themes
        static IconTheme light();
        static IconTheme dark();
        static IconTheme colorful();
        
    private:
        std::string name_;
        IconConfig themeConfig_;
    };
}

#endif // ICON_WRAPPER_HPP