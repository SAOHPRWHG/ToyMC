cmake_minimum_required(VERSION 3.0)

project(ToyMC CXX C)

set(SOURCES
    Source/Cube/MakeCube.cpp
    Source/DataBase/DataBase.cpp
    Source/Maths/GeneralMaths.cpp
    Source/Utils/Inventory.cpp
    Source/Main.cpp
    Source/World/Map.cpp
    Source/Maths/MatrixMath.cpp
    Source/DataBase/Ring.cpp
    Source/Utils/PromptSign.cpp
    Source/Utils/Tools.cpp
    Source/World/World.cpp
    Source/World/Chunk.cpp
	Source/Worker/Worker.cpp
	Source/Game/Physic.cpp
	Source/Game/KBMouse.cpp
	)

add_executable(
    ToyMC
    ${SOURCES}
    Library/noise/noise.c
    Library/sqlite/sqlite3.c
    Library/tinycthread/tinycthread.c)

add_custom_command(TARGET ToyMC PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Resource/Textures/ 
                       Textures)

add_custom_command(TARGET ToyMC PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Resource/Shaders/ 
                       Shaders)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -O3 -g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -g -Wall")

target_include_directories(ToyMC
    PUBLIC
        Library/include
        Library/noise
        Library/sqlite
        Library/tinycthread)

set(GLAD_DIR 
    ${PROJECT_SOURCE_DIR}/Library/glad
)
add_library("glad" ${GLAD_DIR}/glad.c)

target_include_directories(glad
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Library/include
)

#find Library

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)

if(MINGW)
    add_library(glfw3 STATIC IMPORTED)
    set_property(TARGET glfw3 PROPERTY IMPORTED_LOCATION         ${PROJECT_SOURCE_DIR}/Library/lib/libglfw3.a)
else()
	set(GLFW_ROOT_DIR ${PROJECT_SOURCE_DIR}/Library)
    find_package(GLFW REQUIRED)
	target_link_directories(ToyMC
		PUBLIC
			${PROJECT_SOURCE_DIR}/Library/lib)
endif()


target_link_libraries(ToyMC 
    opengl32
    glu32
    glad
    glfw3	
    ${GLFW_LIBRARIES}
)