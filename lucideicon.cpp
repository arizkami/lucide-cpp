#include "lucideicon.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <algorithm>

namespace LucideIcon {

    // Icon class implementation
    Icon::Icon(const std::string& name, const std::string& pathData)
        : name_(name), pathData_(pathData) {}

    std::string Icon::toSVG() const {
        return buildSVG(IconConfig{});
    }

    std::string Icon::toSVG(const IconConfig& config) const {
        return buildSVG(config);
    }

    std::string Icon::buildSVG(const IconConfig& config) const {
        std::stringstream ss;
        
        ss << "<svg";
        ss << " xmlns=\"http://www.w3.org/2000/svg\"";
        ss << " width=\"" << config.width << "\"";
        ss << " height=\"" << config.height << "\"";
        ss << " viewBox=\"0 0 24 24\"";
        ss << " fill=\"" << config.fill << "\"";
        ss << " stroke=\"" << config.stroke << "\"";
        ss << " stroke-width=\"" << config.strokeWidth << "\"";
        ss << " stroke-linecap=\"" << config.strokeLinecap << "\"";
        ss << " stroke-linejoin=\"" << config.strokeLinejoin << "\"";
        
        if (!config.className.empty()) {
            ss << " class=\"" << config.className << "\"";
        }
        
        if (!config.style.empty()) {
            ss << " style=\"" << config.style << "\"";
        }
        
        ss << ">";
        ss << pathData_;
        ss << "</svg>";
        
        return ss.str();
    }

    // IconRegistry implementation
    IconRegistry& IconRegistry::getInstance() {
        static IconRegistry instance;
        return instance;
    }

    void IconRegistry::registerIcon(const std::string& name, const std::string& pathData) {
        icons_[name] = std::make_shared<Icon>(name, pathData);
    }

    std::shared_ptr<Icon> IconRegistry::getIcon(const std::string& name) const {
        auto it = icons_.find(name);
        return (it != icons_.end()) ? it->second : nullptr;
    }

    bool IconRegistry::hasIcon(const std::string& name) const {
        return icons_.find(name) != icons_.end();
    }

    std::vector<std::string> IconRegistry::getAllIconNames() const {
        std::vector<std::string> names;
        names.reserve(icons_.size());
        
        for (const auto& pair : icons_) {
            names.push_back(pair.first);
        }
        
        std::sort(names.begin(), names.end());
        return names;
    }

    std::string IconRegistry::generateSVG(const std::string& name, const IconConfig& config) const {
        auto icon = getIcon(name);
        return icon ? icon->toSVG(config) : "";
    }

    // Utility functions implementation
    namespace Utils {
        bool loadIconsFromDirectory(const std::string& directory) {
            try {
                auto& registry = IconRegistry::getInstance();
                
                for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".svg") {
                        std::ifstream file(entry.path());
                        if (file.is_open()) {
                            std::string content((std::istreambuf_iterator<char>(file)),
                                              std::istreambuf_iterator<char>());
                            
                            std::string pathData = extractPathDataFromSVG(content);
                            if (!pathData.empty()) {
                                std::string iconName = entry.path().stem().string();
                                registry.registerIcon(iconName, pathData);
                            }
                        }
                    }
                }
                return true;
            } catch (const std::exception& e) {
                return false;
            }
        }

        std::string extractPathDataFromSVG(const std::string& svgContent) {
            std::stringstream result;
            
            // Extract path elements
            std::regex pathRegex(R"(<path[^>]*d="([^"]*)");
            std::regex circleRegex(R"(<circle[^>]*cx="([^"]*)");
            std::regex rectRegex(R"(<rect[^>]*x="([^"]*)");
            
            std::sregex_iterator iter(svgContent.begin(), svgContent.end(), pathRegex);
            std::sregex_iterator end;
            
            // Find all path, circle, rect elements
            std::regex elementRegex(R"(<(path|circle|rect|line|polygon|polyline)[^>]*>)");
            std::sregex_iterator elemIter(svgContent.begin(), svgContent.end(), elementRegex);
            
            for (std::sregex_iterator i = elemIter; i != end; ++i) {
                result << i->str();
            }
            
            return result.str();
        }

        bool isValidIconName(const std::string& name) {
            if (name.empty()) return false;
            
            // Check if name contains only valid characters
            std::regex validName(R"(^[a-zA-Z][a-zA-Z0-9_-]*$)");
            return std::regex_match(name, validName);
        }
    }

    // Predefined icon functions
    // Remove the entire Icons namespace block (lines 146-162)
    // namespace Icons {
    //     std::string star(const IconConfig& config) {
    //         return IconRegistry::getInstance().generateSVG("star", config);
    //     }
    //     ... (remove all icon functions)
    // }
}