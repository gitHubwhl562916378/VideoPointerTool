cmake_minimum_required(VERSION 3.9.0)

project(VideoPointerTool
        LANGUAGES C CXX
        VERSION "7.0.0.11"
        DESCRIPTION "videoplayer tie_ta")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17) 

if (WIN32)
        set(Qt5_DIR C:/Qt/5.15.1/msvc2019_64/lib/cmake/Qt5)
        set(FFMPEG_INCLUDE_DIRS E:/ffmpeg/x64/include)
        link_directories(E:/ffmpeg/x64/lib E:/OpenSSL-Win64/lib E:/OpenSSL-Win64/lib/VC)
        include_directories(E:/ffmpeg/x64/include $ENV{INTELMEDIASDKROOT}/include E:/OpenSSL-Win64/include)
        list(APPEND FFMPEG_LIBRARIES avcodec avdevice avfilter avformat avutil postproc swresample swscale)
        list(APPEND EXTERN_LIBRARIES ws2_32 libssl libcrypto)
        if(CMAKE_BUILD_TYPE STREQUAL Debug)
                set(Poco_DIR E:/poco-1.10.1/x64/debug/cmake)
                list(APPEND EXTERN_LIBRARIES libcrypto64MDd libssl64MDd)
        else()
                set(Poco_DIR E:/poco-1.10.1/x64/release/cmake)
                list(APPEND EXTERN_LIBRARIES libcrypto64MD libssl64MD)
        endif ()
elseif (UNIX)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(PC_AVDEVICE REQUIRED libavdevice)
        pkg_check_modules(PC_AVFORMAT REQUIRED libavformat)
        pkg_check_modules(PC_AVUTIL REQUIRED libavutil)
        pkg_check_modules(PC_AVCODEC REQUIRED libavcodec)
        pkg_check_modules(PC_SWSCALE REQUIRED libswscale)
        find_library(AVDEVICE_LIBRARY avdevice HINTS ${PC_AVDEVICE_LIBDIR} ${PC_AVDEVICE_LIBRARY_DIRS})
        find_library(AVFORMAT_LIBRARY avformat HINTS ${PC_AVFORMAT_LIBDIR} ${PC_AVFORMAT_LIBRARY_DIRS})
        find_library(AVUTIL_LIBRARY avutil HINTS ${PC_AVUTIL_LIBDIR} ${PC_AVUTIL_LIBRARY_DIRS})
        find_library(AVCODEC_LIBRAR avcodec HINTS ${PC_AVCODEC_LIBDIR} ${PC_AVCODEC_LIBRARY_DIRS})
        find_library(SWSCALE_LIBRAR swscale HINTS ${PC_SWSCALE_LIBDIR} ${PC_SWSCALE_LIBRARY_DIRS})
        list(APPEND FFMPEG_INCLUDE_DIRS ${PC_AVFORMAT_INCLUDEDIR})
        list(APPEND FFMPEG_LIBRARIES ${AVDEVICE_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVCODEC_LIBRAR} ${SWSCALE_LIBRAR})
endif ()
find_package(Poco REQUIRED Net JSON)
find_package(Qt5 REQUIRED Widgets Core Gui Concurrent)
find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON) #自动生成moc文件
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#查找当前文件夹中的所有源代码文件，也可以通过Set命令将所有文件设置为一个变量
FILE(GLOB ROOT_SRC_FILES *.cpp)
FILE(GLOB_RECURSE VIDEO_SRC_FILES VideoWidget/*.cpp)
FILE(GLOB_RECURSE SERVICE_SRC_FILES Service/*.cpp)
FILE(GLOB_RECURSE DAO_SRC_FILES Dao/*.cpp)
list(APPEND SRC_FILES ${ROOT_SRC_FILES} ${VIDEO_SRC_FILES} ${SERVICE_SRC_FILES} ${DAO_SRC_FILES})
#同理，查找当前文件夹中所有的头文件
FILE(GLOB ROOT_HEAD_FILES *.h)
FILE(GLOB_RECURSE VIDEO_HEADER_FILES VideoWidget/*.h)
FILE(GLOB_RECURSE SERVICE_HEADER_FILES Service/*.h)
FILE(GLOB_RECURSE DAO_HEADER_FILES Dao/*.h)
list(APPEND HEAD_FILES ${ROOT_HEAD_FILES} ${VIDEO_HEADER_FILES} ${SERVICE_HEADER_FILES} ${DAO_HEADER_FILES})
#同理，查找当前文件夹中所有的ui文件
FILE(GLOB UI_FILES *.ui)

#通过Ui文件生成对应的头文件，一定要添加
# qt5_wrap_ui(WRAP_FILES ${UI_FILES})

#添加资源文件
# set(RCC_FILES rcc.qrc)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES} ${UI_HEAD_FILES} ${RCC_FILES} ${WRAP_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Concurrent)
target_link_libraries(${PROJECT_NAME} ${Poco_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${EXTERN_LIBRARIES})
