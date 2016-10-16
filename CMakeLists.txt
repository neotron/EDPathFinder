cmake_minimum_required(VERSION 3.5)
project(EDPathinder)

set(ORTOOLS ../or-tools/)
set(PFDIR deps/Pathfinder/src/)

link_directories(${ORTOOLS}/lib)

set(QT_USE_QTNETWORK true)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)

set(LINK_LIBS ortools Qt5::Widgets Qt5::Network Qt5::Concurrent)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -mmacosx-version-min=10.8 -stdlib=libc++  -DARCH_K8 -Wno-deprecated ")
    link_libraries(${LINK_LIBS} -lz)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
    link_libraries(${LINK_LIBS})
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic  -Wno-padded -Wno-old-style-cast -Wno-double-promotion -Wno-undefined-reinterpret-cast -Wno-implicit-fallthrough -Wno-exit-time-destructors -Wno-global-constructors")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
endif ()

include_directories(SYSTEM ${ORTOOLS}/include)
include_directories(deps/Pathfinder/src/)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PATH_FINDER_SRC ${PFDIR}/AStar.cpp ${PFDIR}/AStar.h ${PFDIR}/Dijkstra.cpp ${PFDIR}/Dijkstra.h ${PFDIR}/Node.cpp ${PFDIR}/Node.h ${PFDIR}/PathAlgorithm.h ${PFDIR}/PathFinder.h)

set(SOURCE_FILES ${PATH_FINDER_SRC} src/main.cpp src/System.cpp src/System.h src/TSPWorker.cpp src/MainWindow.cpp src/RouteViewer.cpp src/RouteViewer.ui src/MainWindow.ui resources/resources.qrc src/RouteTableModel.cpp src/RouteTableModel.h src/NoBeepLineEdit.h src/EDSMQueryExecutor.cpp src/EDSMQueryExecutor.h src/QCompressor.cpp src/QCompressor.h src/AStarRouter.cpp src/AStarRouter.h src/NoBeepLineEdit.cpp)
add_executable(EDPathfinder ${SOURCE_FILES})
