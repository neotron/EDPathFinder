import Unbox

struct Coords {
    let x: Float
    let y: Float
    let z: Float
}
extension Coords: Unboxable {
    init(unboxer: Unboxer) throws {
        self.x = try unboxer.unbox(key: "x")
        self.y = try unboxer.unbox(key: "y")
        self.z = try unboxer.unbox(key: "z")
    }
}
struct System {
    let name: String
    let typeName: String
    let coords: Coords
}

extension System: Unboxable {
    init(unboxer: Unboxer) throws {
        self.name = try unboxer.unbox(key: "name")
        self.typeName = try unboxer.unbox(key: "type")
        self.coords = try unboxer.unbox(key: "coords")
    }
}
let path = CommandLine.arguments[1];
let data = try Data(contentsOf: URL(fileURLWithPath:path), options: .alwaysMapped)

let systems: [System] = try unbox(data: data)

let systemStrings = systems.map{"\($0.name)\t\($0.coords.x)\t\($0.coords.y)\t\($0.coords.z)"}.joined(separator: "\n")

let toFile = "\(path).txt"
try systemStrings.write(toFile: toFile, atomically: true, encoding: .utf8)

