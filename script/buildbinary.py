#!/usr/bin/env python3
"""
Lucide C++ Icon Library Build Script

This script:
1. Scans the lucide/icons directory for SVG files
2. Extracts path data from each SVG
3. Generates C++ code for icon registration
4. Creates optimized binary data for icons
"""

import os
import re
import json
from pathlib import Path
from typing import Dict, List, Tuple
import argparse

# C++ keywords and reserved words that cannot be used as function names
CPP_KEYWORDS = {
    'alignas', 'alignof', 'and', 'and_eq', 'asm', 'atomic_cancel', 'atomic_commit',
    'atomic_noexcept', 'auto', 'bitand', 'bitor', 'bool', 'break', 'case', 'catch',
    'char', 'char8_t', 'char16_t', 'char32_t', 'class', 'compl', 'concept', 'const',
    'consteval', 'constexpr', 'constinit', 'const_cast', 'continue', 'co_await',
    'co_return', 'co_yield', 'decltype', 'default', 'delete', 'do', 'double',
    'dynamic_cast', 'else', 'enum', 'explicit', 'export', 'extern', 'false',
    'float', 'for', 'friend', 'goto', 'if', 'inline', 'int', 'long', 'mutable',
    'namespace', 'new', 'noexcept', 'not', 'not_eq', 'nullptr', 'operator', 'or',
    'or_eq', 'private', 'protected', 'public', 'reflexpr', 'register',
    'reinterpret_cast', 'requires', 'return', 'short', 'signed', 'sizeof',
    'static', 'static_assert', 'static_cast', 'struct', 'switch', 'synchronized',
    'template', 'this', 'thread_local', 'throw', 'true', 'try', 'typedef',
    'typeid', 'typename', 'union', 'unsigned', 'using', 'virtual', 'void',
    'volatile', 'wchar_t', 'while', 'xor', 'xor_eq'
}

def extract_svg_content(svg_file: Path) -> str:
    """Extract the inner content of an SVG file."""
    try:
        with open(svg_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Extract everything between <svg> tags, excluding the svg tag itself
        pattern = r'<svg[^>]*>([\s\S]*?)</svg>'
        match = re.search(pattern, content)
        
        if match:
            return match.group(1).strip()
        return ""
    except Exception as e:
        print(f"Error reading {svg_file}: {e}")
        return ""

def safe_function_name(icon_name: str) -> str:
    """Convert icon name to a safe C++ function name."""
    # Convert kebab-case to snake_case
    func_name = icon_name.replace('-', '_')
    
    # If it's a C++ keyword, append underscore
    if func_name in CPP_KEYWORDS:
        func_name += '_'
    
    return func_name

def generate_icon_registry_code(icons: Dict[str, str]) -> str:
    """Generate C++ code for icon registration."""
    code = []
    code.append("// Auto-generated icon registration code")
    code.append("#include \"../lucideicon.hpp\"")
    code.append("")
    code.append("namespace LucideIcon {")
    code.append("    namespace Generated {")
    code.append("        void registerAllIcons() {")
    code.append("            auto& registry = IconRegistry::getInstance();")
    code.append("")
    
    for icon_name, svg_content in sorted(icons.items()):
        # Escape quotes in SVG content
        escaped_content = svg_content.replace('"', '\\"')
        code.append(f'            registry.registerIcon("{icon_name}", R"({escaped_content})");')
    
    code.append("        }")
    code.append("    }")
    code.append("}")
    
    return "\n".join(code)

def generate_icon_functions(icons: Dict[str, str]) -> str:
    """Generate icon function implementations."""
    code = []
    code.append('// Auto-generated icon function implementations')
    code.append('#include "../lucideicon.hpp"')
    code.append("")
    code.append("namespace LucideIcon {")
    code.append("    namespace Icons {")
    
    for icon_name in sorted(icons.keys()):
        # Convert kebab-case to safe function name
        func_name = safe_function_name(icon_name)
        code.append(f"        std::string {func_name}(const IconConfig& config) {{")
        code.append(f'            return IconRegistry::getInstance().generateSVG("{icon_name}", config);')
        code.append("        }")
        code.append("")
    
    code.append("    }")
    code.append("}")
    
    return "\n".join(code)

def generate_header_declarations(icons: Dict[str, str]) -> str:
    """Generate header declarations for icon functions."""
    code = []
    code.append("namespace LucideIcon {")
    code.append("    namespace Icons {")
    code.append("        // Auto-generated icon function declarations")
    
    for icon_name in sorted(icons.keys()):
        func_name = safe_function_name(icon_name)
        code.append(f"        std::string {func_name}(const IconConfig& config = IconConfig{{}});")
    
    code.append("    }")
    code.append("}")
    
    return "\n".join(code)

def main():
    """Main build function."""
    parser = argparse.ArgumentParser(description='Generate Lucide icon C++ code')
    parser.add_argument('--output-dir', type=str, help='Output directory for generated files')
    args = parser.parse_args()
    
    # Get script directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    # Paths
    icons_dir = project_root / "lucide" / "icons"
    
    # Use provided output directory or determine automatically
    if args.output_dir:
        output_dir = Path(args.output_dir)
    else:
        # Check for CMAKE_CURRENT_BINARY_DIR environment variable
        cmake_binary_dir = os.environ.get('CMAKE_CURRENT_BINARY_DIR')
        if cmake_binary_dir:
            output_dir = Path(cmake_binary_dir) / "generated"
        else:
            # Fallback: try to find build directory
            current_dir = Path.cwd()
            if (current_dir / "build").exists():
                output_dir = current_dir / "build" / "generated"
            elif (project_root / "build").exists():
                output_dir = project_root / "build" / "generated"
            else:
                # Create build directory if it doesn't exist
                build_dir = project_root / "build"
                build_dir.mkdir(exist_ok=True)
                output_dir = build_dir / "generated"
    
    # Create output directory with parents
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print(f"Scanning icons in: {icons_dir}")
    print(f"Output directory: {output_dir}")
    
    if not icons_dir.exists():
        print(f"Error: Icons directory not found: {icons_dir}")
        return False
    
    # Scan for SVG files
    icons = {}
    svg_files = list(icons_dir.glob("*.svg"))
    
    print(f"Found {len(svg_files)} SVG files")
    
    for svg_file in svg_files:
        icon_name = svg_file.stem
        svg_content = extract_svg_content(svg_file)
        
        if svg_content:
            icons[icon_name] = svg_content
            print(f"  Processed: {icon_name}")
        else:
            print(f"  Warning: Could not extract content from {icon_name}")
    
    print(f"Successfully processed {len(icons)} icons")
    
    # Generate files
    print("Generating C++ files...")
    
    # Generate icon registry
    registry_code = generate_icon_registry_code(icons)
    with open(output_dir / "icon_registry.cpp", 'w', encoding='utf-8') as f:
        f.write(registry_code)
    
    # Generate icon functions
    functions_code = generate_icon_functions(icons)
    with open(output_dir / "icon_functions.cpp", 'w', encoding='utf-8') as f:
        f.write(functions_code)
    
    # Generate header declarations
    header_code = generate_header_declarations(icons)
    with open(output_dir / "icon_declarations.hpp", 'w', encoding='utf-8') as f:
        f.write(header_code)
    
    # Generate icon list JSON for reference
    icon_list = {
        "total_icons": len(icons),
        "icons": sorted(icons.keys())
    }
    
    with open(output_dir / "icon_list.json", 'w', encoding='utf-8') as f:
        json.dump(icon_list, f, indent=2)
    
    print(f"Generated files in: {output_dir}")
    print("  - icon_registry.cpp")
    print("  - icon_functions.cpp")
    print("  - icon_declarations.hpp")
    print("  - icon_list.json")
    
    return True

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)