cmake_minimum_required(VERSION 3.16)
project(LucideIconExample VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the example executable
add_executable(lucide-example src/main.cc)

# Link against the parent project's library
target_link_libraries(lucide-example PRIVATE lucide-icon)

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(lucide-example PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Set console subsystem for Windows
    if(MSVC)
        set_target_properties(lucide-example PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    endif()
else()
    # Linux/Unix-specific settings
    target_compile_options(lucide-example PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# Install the example (optional)
install(TARGETS lucide-example
    RUNTIME DESTINATION bin
)