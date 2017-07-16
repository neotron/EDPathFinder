cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ../${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(EDPathFinderTools VERSION 1.0)

add_custom_target(jsonconverter
        COMMAND /Library/Developer/Toolchains/swift-latest.xctoolchain/usr/bin/swift build  -c release
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/jsonconverter/
        SOURCES jsonconverter/sources/main.swift jsonconverter/package.swift)


add_custom_target(coordinate-mapper
        COMMAND /Library/Developer/Toolchains/swift-latest.xctoolchain/usr/bin/swift build  -c release
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/coordinate-mapper/
        SOURCES coordinate-mapper/sources/main.swift coordinate-mapper/package.swift)

add_custom_target(coordinate-mapper-debug
        COMMAND /Library/Developer/Toolchains/swift-latest.xctoolchain/usr/bin/swift build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/coordinate-mapper/
        SOURCES coordinate-mapper/sources/main.swift coordinate-mapper/package.swift)
