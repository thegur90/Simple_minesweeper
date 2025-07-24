cmake_minimum_required(VERSION 3.10)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)

include_directories(C:/msys64/mingw64/include/SDL2)
link_directories(C:/msys64/mingw64/lib)

add_executable(Minesweeper main.cpp util_renderimage.cpp util_modifystringdata.cpp util_aspectratio.cpp util_settings.cpp config_window.cpp)

set_target_properties(Minesweeper PROPERTIES WIN32_EXECUTABLE TRUE)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(Minesweeper mingw32 SDL2main SDL2 SDL2_image SDL2_ttf )