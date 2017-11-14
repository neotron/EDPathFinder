cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ../${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(EDPathFinderTools VERSION 1.0)
#set(CMAKE_VERBOSE_MAKEFILE  YES)

add_custom_target(jsonconverter
        COMMAND swift build  -c release
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/jsonconverter/
        SOURCES jsonconverter/Sources/main.swift jsonconverter/Package.swift)


add_custom_target(coordinate-mapper
        COMMAND swift build -c release
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/coordinate-mapper/
        SOURCES coordinate-mapper/Sources/main.swift coordinate-mapper/Package.swift)

add_custom_target(coordinate-mapper-debug
        COMMAND swift build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/coordinate-mapper/
        SOURCES coordinate-mapper/Sources/main.swift coordinate-mapper/Package.swift)
