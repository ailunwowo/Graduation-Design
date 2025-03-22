cmake_minimum_required(VERSION 3.10)
project(lvgl)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# rlottie 配置
include_directories(/home/alen/VisonFive2_SDK/gif_anmin_so/rlottie/inc)
set(RLOTTIE_LIB_PATH "/home/alen/VisonFive2_SDK/VisionFive2/work/buildroot_rootfs/host/lib")
find_library(RLOTTIE_LIB rlottie HINTS ${RLOTTIE_LIB_PATH})

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(main main.c mouse_cursor_icon.c)

include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)
include_directories(${Libdrm_INCLUDE_DIRS})

find_package(SDL2)
find_package(SDL2_image)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# 链接 rlottie
target_link_libraries(main lvgl lvgl::examples lvgl::demos lvgl::thorvg ${RLOTTIE_LIB} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${Libdrm_LIBRARIES} m pthread)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)