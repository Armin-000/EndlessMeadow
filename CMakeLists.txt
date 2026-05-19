cmake_minimum_required(VERSION 3.16)

project(EndlessMeadow CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(EndlessMeadow
    MACOSX_BUNDLE

    src/main.cpp
    src/world.cpp
    src/alien.cpp
    src/sky.cpp
    src/water.cpp
    src/terrain.cpp
    src/vegetation.cpp
    src/collisions.cpp
    src/map.cpp
    src/inventory.cpp
    src/item_drops.cpp
    src/preload.cpp

    assets/icons/meadow.icns
)

target_include_directories(EndlessMeadow PRIVATE
    include
)

set_source_files_properties(assets/icons/meadow.icns PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)

set_target_properties(EndlessMeadow PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_ICON_FILE meadow.icns
)

target_link_libraries(EndlessMeadow raylib)

if (WIN32)
    target_link_libraries(EndlessMeadow winmm)
endif()

add_custom_command(TARGET EndlessMeadow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:EndlessMeadow>/assets
)