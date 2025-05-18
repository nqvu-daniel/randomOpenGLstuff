cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings and flags
add_compile_options(-Wall -Wno-deprecated -fdiagnostics-color=always -g)

# --- ImGui Library Setup ---
# Corrected paths for ImGui backend files based on user's clang++ command
set(IMGUI_VENDOR_DIR ${CMAKE_SOURCE_DIR}/src/vendor/imgui)
set(IMGUI_SOURCES
    "${IMGUI_VENDOR_DIR}/imgui.cpp"
    "${IMGUI_VENDOR_DIR}/imgui_draw.cpp"
    "${IMGUI_VENDOR_DIR}/imgui_tables.cpp"
    "${IMGUI_VENDOR_DIR}/imgui_widgets.cpp"
    # "${IMGUI_VENDOR_DIR}/imgui_demo.cpp" # Uncomment if you use/compile the ImGui demo
    # Corrected paths: assuming backends are directly in imgui folder, not a subfolder
    "${IMGUI_VENDOR_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_VENDOR_DIR}/imgui_impl_opengl3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_VENDOR_DIR}) # For imgui.h etc.
# GLFW headers for ImGui backends will be added platform-specifically below

# --- Source Files for App ---
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/glad.c
)
add_executable(app ${SRC_FILES})
target_link_libraries(app PRIVATE imgui) # Link app against ImGui

# --- Platform-Specific Configuration ---
if(APPLE)
    message(STATUS "Configuring for macOS using direct paths and frameworks")

    # For ImGui: Add GLFW header directory
    target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW/glfw3.h
    )

    # For App: Include directories
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW, GLAD
        ${CMAKE_SOURCE_DIR}/src/vendor          # For GLM, stb_image
        ${CMAKE_SOURCE_DIR}/src                 # For app's own headers
    )

    # For App: Link directories (to find glfw.3.4.dylib)
    target_link_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/library)

    # For App: Link libraries
    target_link_libraries(app PRIVATE
        glfw.3.4.dylib # Assumes this is in the linked directory
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )

elseif(UNIX AND NOT APPLE) # Linux
    message(STATUS "Configuring for Linux using direct linking for GLFW")

    # Define the directory where libglfw.so or libglfw.a will be located for Linux
    # IMPORTANT: You need to create this directory and place your GLFW library file here.
    set(LINUX_GLFW_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/dependencies/library/linux_x86_64) # Example path

    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED) # Often needed by GLFW on Linux

    # For ImGui: Add GLFW header directory
    # Assumes GLFW headers are in ${CMAKE_SOURCE_DIR}/dependencies/include/GLFW
    target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/dependencies/include
    )

    # For App: Include directories
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW, GLAD headers
        ${CMAKE_SOURCE_DIR}/src/vendor          # For GLM, stb_image
        ${CMAKE_SOURCE_DIR}/src                 # For app's own headers
    )

    # For App: Link directories (to find libglfw.so or libglfw.a)
    target_link_directories(app PRIVATE ${LINUX_GLFW_LIBRARY_DIR})

    # For App: Link libraries
    target_link_libraries(app PRIVATE
        glfw                # CMake will search for libglfw.so or libglfw.a
        ${OPENGL_LIBRARIES}
        ${X11_LIBRARIES}
        Threads::Threads    # Modern CMake for pthread
        dl                  # For dlopen, dlsym etc.
    )
else()
    message(WARNING "Platform-specific linking for GLFW/OpenGL not fully configured for this system. Only macOS and Linux are explicitly supported with this configuration.")
endif()

# --- Common Post-Platform Configuration ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the source resource directory
set(RESOURCE_SRC_DIR ${CMAKE_SOURCE_DIR}/res)

# Add a custom command to copy the resource directory
add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${RESOURCE_SRC_DIR}"
            "$<TARGET_FILE_DIR:app>/res" # Copies 'res' into the directory containing 'app'
    COMMENT "Copying resources to build directory"
)