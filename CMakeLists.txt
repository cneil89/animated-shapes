cmake_minimum_required(VERSION 3.20)

project(SFMLTest
    VERSION 0.1.0 
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2a4b9dd9b34df61d91e33667dbaf5902c4923867)
FetchContent_MakeAvailable(ImGui-SFML)

add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS  _CRT_SECURE_NO_WARNINGS)

set(SRC_FILES 
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} 
    PUBLIC sfml-main 
    PUBLIC sfml-graphics 
    PUBLIC sfml-window 
    PUBLIC sfml-system
    PUBLIC ImGui-SFML::ImGui-SFML
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})