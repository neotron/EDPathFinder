cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include Qt basic functions
include(QtCommon)

#set(CMAKE_VERBOSE_MAKEFILE  YES)

project(EDPathFinder VERSION 1.7.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MSVC_RUNTIME "dynamicmak")

fix_project_version()
configure_msvc_runtime()

set(CMAKE_USE_RELATIVE_PATHS ON)
set(RESOURCE_FOLDER resources)
set(ICON_NAME icon512)
set(COMPANY "David Hedbor")
set(COPYRIGHT "Copyright (c) 2016-2018 David Hedbor. All rights reserved.")
set(IDENTIFIER "org.hedbor.EDPathFinder")

file(GLOB_RECURSE RESOURCE_FILES resources/*.qrc)

add_project_meta(META_FILES_TO_INCLUDE)

set(ORTOOLS ${PROJECT_SOURCE_DIR}/../or-tools)
set(PFDIR ${PROJECT_SOURCE_DIR}/deps/PathFinder/src/)
set(JOURNALDIR ${PROJECT_SOURCE_DIR}/deps/EDJournalQT/src/)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_USE_QTNETWORK true)
find_package(Qt5Core REQUIRED)
#find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5TextToSpeech REQUIRED)
add_subdirectory(deps/EDJournalQT)

set(LINK_LIBS ortools Qt5::Core Qt5::Widgets Qt5::Network Qt5::Concurrent Qt5::TextToSpeech EDJournalQT)

link_directories(${ORTOOLS}/lib)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    add_definitions(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic  -Wno-padded
    -Wno-old-style-cast -Wno-double-promotion -Wno-undefined-reinterpret-cast -Wno-implicit-fallthrough
    -Wno-exit-time-destructors -Wno-global-constructors -Wno-switch-enum -Wno-redundant-parens)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
endif ()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mmacosx-version-min=10.9 -stdlib=libc++ -Wno-import-preprocessor-directive-pedantic")
    link_libraries(${LINK_LIBS} -lz)
endif ()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCH_K8 -Wno-deprecated ")
    find_library(FOUNDATION Foundation)
    link_libraries(${LINK_LIBS} -lz ${FOUNDATION})

    file(GLOB_RECURSE MM_FILES src/*.mm)
else (MSVC)
    set(WINTOAST "deps/wintoast/src/wintoastlib.cpp" "deps/wintoast/src/wintoastlib.h")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    link_libraries(${LINK_LIBS})
endif ()

file(GLOB_RECURSE PATH_FINDER_SRC ${PFDIR}/*.cpp ${PFDIR}/*.h )
file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h src/*.ui)

header_directories(INCLUDE_DIRS src "${PROJECT_SOURCE_DIR}")
include_directories(SYSTEM ${ORTOOLS}/include/ deps/PathFinder/src/ )
include_directories(AFTER ${INCLUDE_DIRS} "deps/EDJournalQT/src")
add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
    ${SRC_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES} ${PATH_FINDER_SRC} ${WINTOAST} ${MM_FILES} ${DOUTPUT}
)

add_executable(bodydistance tools/bodydistance/main.cpp src/Model/System.cpp src/Model/SystemLoader.cpp src/Routers/Tools/AStarRouter.cpp ${PATH_FINDER_SRC} ${RESOURCE_FILES})

configure_file("${PROJECT_SOURCE_DIR}/Makefile.in"  "${PROJECT_SOURCE_DIR}/Makefile")
configure_file("${PROJECT_SOURCE_DIR}/src/buildnumber.h.in" "${PROJECT_BINARY_DIR}/buildnumber.h")

if  (MSVC)
    # copy dlls.
    configure_file("deps/openssl/ssleay32.dll" "${PROJECT_BINARY_DIR}/" COPYONLY)
    configure_file("deps/openssl/libeay32.dll" "${PROJECT_BINARY_DIR}/" COPYONLY)

    add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD COMMAND "C:/User Programs/Qt/${Qt5Widgets_VERSION}/msvc2017_64/bin/windeployqt.exe" --verbose 1 "${PROJECT_NAME}.exe"
            WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endif ()
