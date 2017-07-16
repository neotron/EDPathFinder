// swift-tools-version:3.1

import PackageDescription

let package = Package(
        name: "coordinate-mapper",
        dependencies: [
            .Package(url: "https://github.com/utahiosmac/Marshal", majorVersion: 1, minor: 2),
        ]
)
