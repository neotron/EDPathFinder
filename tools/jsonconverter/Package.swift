import PackageDescription

let package = Package(
name: "jsonconverter",
        dependencies: [
                .Package(url: "https://github.com/JohnSundell/Unbox", majorVersion: 2, minor: 3),
        ]
)
