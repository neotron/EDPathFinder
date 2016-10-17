cmake_minimum_required(VERSION 3.5)
project(EDPathFinder)

set(ORTOOLS ../or-tools/)
set(PFDIR deps/PathFinder/src/)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(LINK_LIBS ortools Qt5::Core Qt5::Widgets Qt5::Network Qt5::Concurrent)

link_directories(${ORTOOLS}/lib)

set(QT_USE_QTNETWORK true)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic  -Wno-padded -Wno-old-style-cast -Wno-double-promotion -Wno-undefined-reinterpret-cast -Wno-implicit-fallthrough -Wno-exit-time-destructors -Wno-global-constructors")
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
    link_libraries(${LINK_LIBS})
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(SYSTEM ${ORTOOLS}/include deps/PathFinder/src/)

file(GLOB PATH_FINDER_SRC ${PFDIR}/*.cpp ${PFDIR}/*.h)

file(GLOB SOURCE_FILES src/*.cpp src/*.h src/*.ui ${PATH_FINDER_SRC})
add_executable(EDPathFinder ${SOURCE_FILES} resources/resources.qrc)
