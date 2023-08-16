cmake_minimum_required (VERSION 3.10)

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

project (PhotoneoDataCollection)

set(CMAKE_BUILD_TYPE RelWithDebInfo )
set(CMAKE_RELEASE_POSTFIX "_Release")
set(CMAKE_DEBUG_POSTFIX "_Debug")
# allow CMake to find our find-script
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

if (UNIX AND NOT APPLE)
    add_compile_options(-std=c++1y)
    add_compile_options(-pthread)
endif(UNIX AND NOT APPLE)

set(Files
    ${PhotoneoDataCollection_SOURCE_DIR}/Calibration.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/ColorPointCloud.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/DepthMap.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/ExternalCamera.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/FileCamera.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/Scanner.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/Util.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/DataCollection.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/main.cpp
    ${PhotoneoDataCollection_SOURCE_DIR}/ReadMe.txt
)

add_executable (PhotoneoDataCollection 
    ${Files}
)

if (NOT PHO_API_CMAKE_CONFIG_PATH)
    set(PHO_API_CMAKE_CONFIG_PATH "$ENV{PHOXI_CONTROL_PATH}")
endif()

find_package(PhoXi REQUIRED CONFIG PATHS "${PHO_API_CMAKE_CONFIG_PATH}")

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package( uEye-SDK 4.95)
find_package(PythonLibs REQUIRED)
find_package(PkgConfig)
pkg_check_modules(YAMLCPP REQUIRED yaml-cpp>=0.5)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${YAMLCPP_INCLUDE_DIRS})

find_package(PCL 1.7 REQUIRED) 

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

include_directories(${PYTHON_INCLUDE_DIRS})

set(PHO_OPENCV_LIBS "world")
find_package(OpenCV QUIET COMPONENTS ${PHO_OPENCV_LIBS} PATHS "${OPEN_CV_PATH}")

if (NOT ${OPENCV_WORLD_FOUND})
    set(PHO_OPENCV_LIBS "core;imgcodecs;imgproc;aruco")
    find_package(OpenCV REQUIRED COMPONENTS ${PHO_OPENCV_LIBS} PATHS "${OPEN_CV_PATH}")
endif()

foreach(PHO_OPENCV_LIB ${PHO_OPENCV_LIBS})
    list(APPEND PHO_TARGET_OPENCV_LIBS "opencv_${PHO_OPENCV_LIB}")
endforeach()

target_link_libraries(PhotoneoDataCollection ${PHO_TARGET_OPENCV_LIBS})

set(OPEN_CV_BIN_PATH "${OpenCV_LIB_DIR_OPT}/../bin")

target_link_libraries(PhotoneoDataCollection
    ${PCL_LIBRARIES}
    ${YAMLCPP_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${PHOXI_LIBRARY}
    ${PYTHON_LIBRARIES}
    stdc++fs
    uEye-SDK
    $<$<PLATFORM_ID:Linux>:rt>
)

get_target_property(ReleaseBinFolder PhotoneoDataCollection RUNTIME_OUTPUT_DIRECTORY_RELEASE)
get_target_property(DebugBinFolder PhotoneoDataCollection RUNTIME_OUTPUT_DIRECTORY_DEBUG)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    if(MSVC_TOOLSET_VERSION GREATER_EQUAL 140)
        set(PHO_COMPILER_VERSION "vc14")
    elseif(MSVC_TOOLSET_VERSION EQUAL 120)
        set(PHO_COMPILER_VERSION "vc12")
    endif()
    
    foreach(PHO_OPENCV_LIB ${PHO_OPENCV_LIBS})
        set(OPEN_CV_RELEASE_DLL_PATH "${OPEN_CV_BIN_PATH}/opencv_${PHO_OPENCV_LIB}310.dll")
        set(OPEN_CV_DEBUG_DLL_PATH "${OPEN_CV_BIN_PATH}/opencv_${PHO_OPENCV_LIB}310d.dll")
        
        if (NOT EXISTS ${OPEN_CV_DEBUG_DLL_PATH})
            set(OPEN_CV_DEBUG_DLL_PATH "${OPEN_CV_BIN_PATH}/opencv_${PHO_OPENCV_LIB}310-gd.dll")
        endif()
        
        add_custom_command(TARGET ExternalCameraExample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<$<CONFIG:Release>:${OPEN_CV_RELEASE_DLL_PATH}>
                $<$<CONFIG:Debug>:${OPEN_CV_DEBUG_DLL_PATH}>
                $<TARGET_FILE_DIR:ExternalCameraExample>
        )
        add_custom_command(TARGET ExternalCameraExample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<$<CONFIG:Release>:${PHOXI_DLL_RELEASE}>
                $<$<CONFIG:Debug>:${PHOXI_DLL_DEBUG}>
                $<TARGET_FILE_DIR:ExternalCameraExample>
        )
    endforeach()
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

target_include_directories(PhotoneoDataCollection PUBLIC ${PHOXI_INCLUDE_DIRS})

set_target_properties(PhotoneoDataCollection
    PROPERTIES
    #for executables, inheritance of variables CMAKE_DEBUG_POSTFIX... does not work
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX}
)
