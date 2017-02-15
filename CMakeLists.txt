cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include Qt basic functions
include(QtCommon)

project(EDPathFinder VERSION 1.0)

fix_project_version()

set(RESOURCE_FOLDER resources)
set(ICON_NAME icon512)
set(COMPANY "David Hedbor")
set(COPYRIGHT "Copyright (c) 2016 David Hedbor. All rights reserved.")
set(IDENTIFIER "org.hedbor.EDPathFinder")

set(RESOURCE_FILES resources/resources.qrc)

add_project_meta(META_FILES_TO_INCLUDE)

set(ORTOOLS ../or-tools/)
set(PFDIR deps/PathFinder/src/)
set(JOURNALDIR deps/EDJournalQT/src/)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_USE_QTNETWORK true)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)


set(LINK_LIBS ortools Qt5::Core Qt5::Widgets Qt5::Network Qt5::Concurrent Qt5::Sql)

link_directories(${ORTOOLS}/lib)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic  -Wno-padded -Wno-old-style-cast -Wno-double-promotion -Wno-undefined-reinterpret-cast -Wno-implicit-fallthrough -Wno-exit-time-destructors -Wno-global-constructors -Wno-switch-enum")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
endif ()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mmacosx-version-min=10.8 -stdlib=libc++ ")
endif ()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -DARCH_K8 -Wno-deprecated ")
    link_libraries(${LINK_LIBS} -lz)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS /W1")
    link_libraries(${LINK_LIBS})
endif ()
file(GLOB EDJOURNAL_SRC ${JOURNALDIR}/*.cpp ${JOURNALDIR}/*.h)
file(GLOB PATH_FINDER_SRC ${PFDIR}/*.cpp ${PFDIR}/*.h)
file(GLOB SOURCE_FILES src/*.cpp src/*.h src/*.ui ${PATH_FINDER_SRC} ${EDJOURNAL_SRC})
include_directories(SYSTEM ${ORTOOLS}/include deps/PathFinder/src/)

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
    ${SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
)
#add_executable(EDPathFinder MACOSX_BUNDLE "${SOURCE_FILES}" resources/resources.qrc ${ICON_FILE})
