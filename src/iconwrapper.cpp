#include "iconwrapper.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cstring>

// Remove this line since it's already defined in CMake
// #define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

namespace LucideIcon {

    // IconWrapper implementation
    IconWrapper::IconWrapper(const std::string& iconName) {
        auto& registry = IconRegistry::getInstance();
        icon_ = registry.getIcon(iconName);
        if (!icon_) {
            throw std::runtime_error("Icon not found: " + iconName);
        }
        config_ = IconConfig{}; // Default configuration
    }

    IconWrapper::IconWrapper(std::shared_ptr<Icon> icon) 
        : icon_(icon), config_(IconConfig{}) {
        if (!icon_) {
            throw std::runtime_error("Invalid icon provided");
        }
    }

    IconWrapper& IconWrapper::size(int width, int height) {
        config_.width = width;
        config_.height = height;
        return *this;
    }

    IconWrapper& IconWrapper::size(int size) {
        return this->size(size, size);
    }

    IconWrapper& IconWrapper::stroke(const std::string& color) {
        config_.stroke = color;
        return *this;
    }

    IconWrapper& IconWrapper::strokeWidth(int width) {
        config_.strokeWidth = width;
        return *this;
    }

    IconWrapper& IconWrapper::fill(const std::string& color) {
        config_.fill = color;
        return *this;
    }

    IconWrapper& IconWrapper::className(const std::string& cls) {
        config_.className = cls;
        return *this;
    }

    IconWrapper& IconWrapper::style(const std::string& styleStr) {
        config_.style = styleStr;
        return *this;
    }

    std::string IconWrapper::render() const {
        if (!icon_) {
            return "";
        }
        return icon_->toSVG(config_);
    }

    IconWrapper& IconWrapper::reset() {
        config_ = IconConfig{};
        return *this;
    }

    IconWrapper IconWrapper::clone() const {
        IconWrapper cloned(icon_);
        cloned.config_ = config_;
        return cloned;
    }

    IconWrapper& IconWrapper::color(const std::string& color) {
        config_.color = color;
        config_.stroke = color;  // Set both for convenience
        config_.fill = color;    // Set both for convenience
        return *this;
    }

    std::vector<std::string> IconWrapper::renderMultiple(
        const std::vector<std::string>& iconNames,
        const IconConfig& config) {
        
        std::vector<std::string> results;
        results.reserve(iconNames.size());
        
        auto& registry = IconRegistry::getInstance();
        
        for (const auto& iconName : iconNames) {
            auto icon = registry.getIcon(iconName);
            if (icon) {
                results.push_back(icon->toSVG(config));
            } else {
                results.push_back(""); // Empty string for missing icons
            }
        }
        
        return results;
    }

    // IconCollection implementation
    IconCollection::IconCollection(const std::string& name) : name_(name) {}

    void IconCollection::addIcon(const std::string& iconName) {
        auto& registry = IconRegistry::getInstance();
        if (registry.hasIcon(iconName)) {
            iconNames_.push_back(iconName);
        }
    }

    void IconCollection::addIcons(const std::vector<std::string>& iconNames) {
        for (const auto& iconName : iconNames) {
            addIcon(iconName);
        }
    }

    void IconCollection::removeIcon(const std::string& iconName) {
        iconNames_.erase(
            std::remove(iconNames_.begin(), iconNames_.end(), iconName),
            iconNames_.end()
        );
    }

    void IconCollection::clear() {
        iconNames_.clear();
    }

    std::vector<std::string> IconCollection::renderAll(const IconConfig& config) const {
        return IconWrapper::renderMultiple(iconNames_, config);
    }

    // IconTheme implementation
    IconTheme::IconTheme(const std::string& name) : name_(name), themeConfig_(IconConfig{}) {}

    IconTheme& IconTheme::setDefaultStroke(const std::string& color) {
        themeConfig_.stroke = color;
        return *this;
    }

    IconTheme& IconTheme::setDefaultFill(const std::string& color) {
        themeConfig_.fill = color;
        return *this;
    }

    IconTheme& IconTheme::setDefaultStrokeWidth(int width) {
        themeConfig_.strokeWidth = width;
        return *this;
    }

    IconTheme& IconTheme::setDefaultSize(int size) {
        themeConfig_.setSize(size);
        return *this;
    }

    IconTheme& IconTheme::setDefaultColor(const std::string& color) {
        themeConfig_.setColor(color);
        return *this;
    }

    // Remove this duplicate definition (lines 170-173):
    // IconTheme& IconTheme::setDefaultSize(int size) {
    //     themeConfig_.width = size;
    //     themeConfig_.height = size;
    // }

    IconConfig IconTheme::applyTheme(const IconConfig& baseConfig) const {
        IconConfig result = baseConfig;
        
        // Apply theme defaults only if base config uses defaults
        if (baseConfig.stroke == "currentColor") {
            result.stroke = themeConfig_.stroke.empty() ? "currentColor" : themeConfig_.stroke;
        }
        if (baseConfig.fill == "none") {
            result.fill = themeConfig_.fill.empty() ? "none" : themeConfig_.fill;
        }
        if (baseConfig.strokeWidth == 2) {
            result.strokeWidth = themeConfig_.strokeWidth == 2 ? 2 : themeConfig_.strokeWidth;
        }
        if (baseConfig.width == 24) {
            result.width = themeConfig_.width == 24 ? 24 : themeConfig_.width;
        }
        if (baseConfig.height == 24) {
            result.height = themeConfig_.height == 24 ? 24 : themeConfig_.height;
        }
        
        return result;
    }

    IconTheme IconTheme::light() {
        IconTheme theme("light");
        theme.setDefaultStroke("#000000");
        theme.setDefaultFill("none");
        theme.setDefaultStrokeWidth(2);
        theme.setDefaultSize(24);
        return theme;
    }

    IconTheme IconTheme::dark() {
        IconTheme theme("dark");
        theme.setDefaultStroke("#ffffff");
        theme.setDefaultFill("none");
        theme.setDefaultStrokeWidth(2);
        theme.setDefaultSize(24);
        return theme;
    }

    IconTheme IconTheme::colorful() {
        IconTheme theme("colorful");
        theme.setDefaultStroke("#3b82f6");
        theme.setDefaultFill("#dbeafe");
        theme.setDefaultStrokeWidth(2);
        theme.setDefaultSize(24);
        return theme;
    }

    // Advanced SVG parsing utilities using nanosvg
    namespace SVGUtils {
        
        struct SVGInfo {
            float width;
            float height;
            std::string pathData;
            bool isValid;
        };
        
        SVGInfo parseSVGString(const std::string& svgContent) {
            SVGInfo info = {0, 0, "", false};
            
            // Create a mutable copy for nanosvg
            std::string mutableSvg = svgContent;
            
            NSVGimage* image = nsvgParse(const_cast<char*>(mutableSvg.c_str()), "px", 96.0f);
            if (!image) {
                return info;
            }
            
            info.width = image->width;
            info.height = image->height;
            info.isValid = true;
            
            // Extract path data from shapes
            std::stringstream pathStream;
            for (NSVGshape* shape = image->shapes; shape != nullptr; shape = shape->next) {
                for (NSVGpath* path = shape->paths; path != nullptr; path = path->next) {
                    pathStream << "<path d=\"";
                    
                    // Convert bezier points to SVG path commands
                    for (int i = 0; i < path->npts - 1; i += 3) {
                        float* p = &path->pts[i * 2];
                        if (i == 0) {
                            pathStream << "M" << p[0] << "," << p[1];
                        }
                        pathStream << "C" << p[2] << "," << p[3] << " "
                                  << p[4] << "," << p[5] << " "
                                  << p[6] << "," << p[7];
                    }
                    
                    if (path->closed) {
                        pathStream << "Z";
                    }
                    
                    pathStream << "\"";
                    
                    // Add stroke and fill attributes from shape
                    if (shape->stroke.type == NSVG_PAINT_COLOR) {
                        unsigned int color = shape->stroke.color;
                        pathStream << " stroke=\"#" << std::hex 
                                  << ((color >> 16) & 0xFF) 
                                  << ((color >> 8) & 0xFF) 
                                  << (color & 0xFF) << "\"" << std::dec;
                        pathStream << " stroke-width=\"" << shape->strokeWidth << "\"";
                    }
                    
                    if (shape->fill.type == NSVG_PAINT_COLOR) {
                        unsigned int color = shape->fill.color;
                        pathStream << " fill=\"#" << std::hex 
                                  << ((color >> 16) & 0xFF) 
                                  << ((color >> 8) & 0xFF) 
                                  << (color & 0xFF) << "\"" << std::dec;
                    } else {
                        pathStream << " fill=\"none\"";
                    }
                    
                    pathStream << "/>";
                }
            }
            
            info.pathData = pathStream.str();
            nsvgDelete(image);
            
            return info;
        }
        
        bool validateSVG(const std::string& svgContent) {
            SVGInfo info = parseSVGString(svgContent);
            return info.isValid;
        }
        
        std::pair<float, float> getSVGDimensions(const std::string& svgContent) {
            SVGInfo info = parseSVGString(svgContent);
            return {info.width, info.height};
        }
    }
}

