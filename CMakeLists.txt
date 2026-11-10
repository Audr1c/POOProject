cmake_minimum_required(VERSION 3.25)


project(POOProject LANGUAGES CXX)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug ou Release" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SFML 2.6 
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)   # liens statiques : pas de .dll a copier
set(SFML_BUILD_AUDIO   OFF CACHE BOOL "" FORCE)  # inutile pour le projet
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SFML
    URL https://github.com/SFML/SFML/archive/refs/tags/2.6.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SYSTEM)                                      # pas d'avertissements venant des en-tetes SFML
FetchContent_MakeAvailable(SFML)

# Moteur physique (sans dependance a SFML)
add_library(physics STATIC
    ./physics/color.cpp
    ./physics/entity.cpp
    ./physics/drawable.cpp
    ./physics/renderer.cpp
    ./physics/vec2.cpp
    ./physics/particles.cpp
    ./physics/world.cpp
    ./physics/forces/force.cpp
    ./physics/forces/springs.cpp
    ./physics/integrators/integrators.cpp)
target_compile_options(physics PRIVATE -Wall -Wextra -Werror)

# Rendu SFML
add_library(render STATIC ./render/sfmlrenderer.cpp)
target_compile_options(render PRIVATE -Wall -Wextra -Werror)
target_link_libraries(render PUBLIC physics sfml-graphics)

# Nos programmes
add_executable(poop ./app/main.cpp)
add_executable(sfml ./app/sfml_test.cpp)
add_executable(world_test ./app/world_test.cpp)

# Options strictes du sujet, appliquees a NOTRE code seulement (pas a SFML)
target_compile_options(poop PRIVATE -Wall -Wextra -Werror)
target_link_libraries(poop PRIVATE physics render sfml-graphics)

target_compile_options(sfml PRIVATE -Wall -Wextra -Werror)
target_link_libraries(sfml PRIVATE sfml-graphics)
