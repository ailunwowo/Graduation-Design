cmake_minimum_required(VERSION 3.10)
project(lvgl)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 新增编译文件夹
file(GLOB APP 
    ${CMAKE_CURRENT_SOURCE_DIR}/app/*.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/app/custom/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/generated/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/generated/guider_customer_fonts/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/generated/guider_fonts/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/generated/images/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app/camare_lvgl.c # 添加摄像头程序
)

include_directories(
    ./lvgl_lib/inc 
    ${PROJECT_SOURCE_DIR}/app 
    ${PROJECT_SOURCE_DIR}/app/custom
    ${PROJECT_SOURCE_DIR}/app/generated
    ${PROJECT_SOURCE_DIR}/app/generated/guider_customer_fonts 
    ${PROJECT_SOURCE_DIR}/app/generated/guider_fonts 
    ${PROJECT_SOURCE_DIR}/app/generated/images 
    /home/alen/VisonFive2_SDK/VisionFive2/work/buildroot_rootfs/host/riscv64-buildroot-linux-gnu/sysroot/usr/include # libjpeg 头文件路径
)

# rlottie 配置
set(RLOTTIE_LIB_PATH "./lvgl_lib")
find_library(RLOTTIE_LIB rlottie HINTS ${RLOTTIE_LIB_PATH})

# libjpeg 配置
set(LIBJPEG_LIB_PATH "/home/alen/VisonFive2_SDK/VisionFive2/work/buildroot_rootfs/host/riscv64-buildroot-linux-gnu/sysroot/usr/lib")
find_library(LIBJPEG_LIB jpeg HINTS ${LIBJPEG_LIB_PATH})
if(NOT LIBJPEG_LIB)
    message(FATAL_ERROR "libjpeg not found in ${LIBJPEG_LIB_PATH}")
endif()

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(main main.c mouse_cursor_icon.c ${APP})

include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)
include_directories(${Libdrm_INCLUDE_DIRS})

find_package(SDL2)
find_package(SDL2_image)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# 链接库
target_link_libraries(main 
    lvgl 
    lvgl::examples 
    lvgl::demos 
    lvgl::thorvg 
    ${RLOTTIE_LIB} 
    ${LIBJPEG_LIB} 
    ${SDL2_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES} 
    ${Libdrm_LIBRARIES} 
    m 
    pthread
)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)