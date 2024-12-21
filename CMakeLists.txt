cmake_minimum_required(VERSION 3.16)

project(iolite)

add_compile_options("$<$<CONFIG:Debug>:-DIOL_DEBUG>")
add_compile_options("$<$<CONFIG:Release>:-DIOL_RELEASE>")

SET (src_path "src")

# -----------------------------------------------------------------------------------------------
# Executable Target
# -----------------------------------------------------------------------------------------------

file(GLOB_RECURSE sources "${src_path}/*.c" "${src_path}/*.cpp" "${src_path}/*.h" "${src_path}/*.hpp" "${src_path}/*.glsl")
add_executable(iolite ${sources})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${sources})

set_target_properties(iolite PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/game/"
)

set_property(TARGET iolite PROPERTY CXX_STANDARD 17)
set_property(TARGET iolite PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------------------------
# Include Directories
# -----------------------------------------------------------------------------------------------

target_include_directories(iolite PUBLIC ${src_path}/engine/include/)
target_include_directories(iolite PUBLIC ${src_path}/engine/include/thirdparty/)

#target_include_directories(iolite PUBLIC /usr/include/SDL2/)
target_include_directories(iolite PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/SDL2/include/)

target_include_directories(iolite PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/glew/include/)

# -----------------------------------------------------------------------------------------------
# Libraries
# -----------------------------------------------------------------------------------------------

#add_library(GL SHARED IMPORTED)
#set_target_properties(GL PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libGL.so)
#target_link_libraries(iolite GL)

find_package(OpenGL REQUIRED)
target_link_libraries(iolite opengl32)

add_library(GLEW SHARED IMPORTED)
#set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libGLEW.so)
set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll)
set_target_properties(GLEW PROPERTIES IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/thirdparty/glew/lib/Release/x64/glew32.lib)
target_link_libraries(iolite GLEW)

add_library(SDL2 SHARED IMPORTED)
#set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libSDL2.so)
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.dll)
set_target_properties(SDL2 PROPERTIES IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.lib)
target_link_libraries(iolite SDL2)

# -----------------------------------------------------------------------------------------------
# Compile Definitions
# -----------------------------------------------------------------------------------------------

add_compile_definitions(SDL_MAIN_HANDLED IOL_OPENGL GLM_FORCE_XYZW_ONLY)

# -----------------------------------------------------------------------------------------------
# Compile Options, disable Warnings
# -----------------------------------------------------------------------------------------------

#if(MSVC)
#  target_compile_options(iolite PRIVATE /W4 /WX)
#else()
#  target_compile_options(iolite PRIVATE -Wall -Wextra -Wpedantic -Werror)
#endif()

add_definitions("/wd26495") # A member variable isn't initialized by a constructor or by an initializer. Make sure all variables are initialized by the end of construction

# -----------------------------------------------------------------------------------------------
# Copy dlls
# -----------------------------------------------------------------------------------------------

if(WIN32)
	#file(GLOB_RECURSE dlls "${CMAKE_SOURCE_DIR}/thirdparty/*.dll")
	SET (dlls
		"${CMAKE_SOURCE_DIR}/thirdparty/SDL2/lib/x64/SDL2.dll"
		"${CMAKE_SOURCE_DIR}/thirdparty/glew/bin/Release/x64/glew32.dll"
	)

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${dlls}
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()