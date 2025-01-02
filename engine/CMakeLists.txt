# Find source files
file(GLOB_RECURSE engine_sources "*.c" "*.cpp" "*.h" "*.hpp" "*.glsl")

# Define the engine project
add_library(engine STATIC ${engine_sources})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${engine_sources})

# Include the engine's include directory
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/thirdparty/)

# Add third-party dependencies
find_package(OpenGL REQUIRED)
target_link_libraries(engine PRIVATE opengl32)

add_library(GLEW SHARED IMPORTED)
set_target_properties(GLEW PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll
    IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/thirdparty/glew/lib/Release/x64/glew32.lib
)
target_link_libraries(engine PRIVATE GLEW)

add_library(SDL2 SHARED IMPORTED)
set_target_properties(SDL2 PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.dll
    IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.lib
)
target_link_libraries(engine PRIVATE SDL2)

# Add compile definitions
target_compile_definitions(engine PRIVATE SDL_MAIN_HANDLED IOL_OPENGL GLM_FORCE_XYZW_ONLY)

# Copy dlls to output folder
if(WIN32)
    set(dlls
        "${CMAKE_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll"
        "${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.dll"
    )

    # Post-build command to copy DLLs to the output directory (Debug/Release)
    add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dlls}
        $<IF:$<CONFIG:Debug>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG},${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>
    )
endif()