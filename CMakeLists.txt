cmake_minimum_required(VERSION 3.16)

project(EndlessMeadow CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(EndlessMeadow
    MACOSX_BUNDLE
    main.cpp
    meadow.icns
)

set_source_files_properties(meadow.icns PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)

set_target_properties(EndlessMeadow PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_ICON_FILE meadow.icns
)

target_link_libraries(EndlessMeadow raylib)

add_custom_command(TARGET EndlessMeadow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:EndlessMeadow>/assets
)