// swift-tools-version:4.0

import PackageDescription

let package = Package(
        name: "coordinate-mapper",
        dependencies: [
            .package(url: "https://github.com/JohnSundell/Unbox", from: "2.3.0"),  
        ],
        targets: [
            .target(name: "coordinate-mapper"),
            .target(name: "jsonconverter", dependencies: ["Unbox"]), 
        ]
)