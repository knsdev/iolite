# Find source files
file(GLOB_RECURSE game_sources "*.c" "*.cpp" "*.h" "*.hpp" "*.glsl")

# Define the game project
add_executable(game ${game_sources})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${game_sources})

# Link the engine library
target_link_libraries(game PRIVATE engine)

# Set the working directory for debugging in Visual Studio
set_target_properties(game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
)