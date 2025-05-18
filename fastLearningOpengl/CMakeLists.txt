cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wno-deprecated -fdiagnostics-color=always -g)

# --- Source Files for App ---
file(GLOB_RECURSE APP_SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
# Add glad.c explicitly as it's a .c file
set(APP_SRC_FILES ${APP_SRC_FILES} "${CMAKE_SOURCE_DIR}/glad.c")

add_executable(app ${APP_SRC_FILES})

# --- Include Directories for App ---
target_include_directories(app PRIVATE
    "${CMAKE_SOURCE_DIR}/src"                 # App's own headers
    "${CMAKE_SOURCE_DIR}/src/vendor"          # For GLM, stb_image
    "${CMAKE_SOURCE_DIR}/dependencies/include" # For GLAD headers
)

# --- ImGui Library Setup ---
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/src/vendor/imgui)
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_impl_glfw.cpp"    # Path based on your structure
    "${IMGUI_DIR}/imgui_impl_opengl3.cpp" # Path based on your structure
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR}) # For imgui.h itself
set_target_properties(imgui PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

# --- Main Dependencies ---
find_package(OpenGL REQUIRED) # Common for all platforms

# --- Platform-Specific Configuration ---
if(APPLE)
    message(STATUS "Configuring for macOS")

    # For ImGui: Add GLFW header directory
    target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW/glfw3.h
    )
    # For App: Add GLFW header directory
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW headers
    )

    # For App: Link directories (to find glfw.3.4.dylib)
    target_link_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/library)

    # For App: Link libraries
    target_link_libraries(app PRIVATE
        imgui # Link ImGui to app
        glfw.3.4.dylib # Assumes this is in the linked directory
        OpenGL::GL # Modern CMake for OpenGL
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )

elseif(UNIX AND NOT APPLE) # Linux
    message(STATUS "Configuring for Linux using GLFW from dependencies directory")
    # find_package(GLFW3 REQUIRED) # Removed: We are using local GLFW
    find_package(X11 REQUIRED)   # Still needed by GLFW on Linux
    find_package(Threads REQUIRED) # Added to find pthreads

    # For ImGui: Add GLFW header directory from dependencies
    target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW/glfw3.h
    )
    # For App: Add GLFW header directory from dependencies
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/include # For GLFW, GLAD headers
        ${CMAKE_SOURCE_DIR}/src/vendor          # For GLM, stb_image
        ${CMAKE_SOURCE_DIR}/src                 # For app's own headers
    )

    # For App: Link directories (to find libglfw.so or similar)
    target_link_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/library)

    # For App: Link libraries
    target_link_libraries(app PRIVATE
        imgui # Link ImGui to app
        glfw  # Link against libglfw.so (or .a) found in dependencies/library
        OpenGL::GL # Modern CMake for OpenGL
        ${X11_LIBRARIES}
        Threads::Threads # Modern CMake for pthread
        dl               # For dlopen, dlsym etc.
    )
else()
    message(WARNING "Platform-specific linking not fully configured for this system. Only macOS and Linux are explicitly supported with this configuration.")
endif()

# --- Common Post-Platform Configuration ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the source resource directory and copy command (retained from your original)
set(RESOURCE_SRC_DIR ${CMAKE_SOURCE_DIR}/res)
add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${RESOURCE_SRC_DIR}"
            "$<TARGET_FILE_DIR:app>/res" # Copies 'res' into the directory containing 'app'
    COMMENT "Copying resources to build directory"
)