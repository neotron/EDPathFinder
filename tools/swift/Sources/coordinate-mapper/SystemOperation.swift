//
// Created by David Hedbor on 2017-07-16.
//

import Foundation

struct Coords: Decodable {
    var x: Float
    var y: Float
    var z: Float
}
struct SystemObj: Decodable {

    var id64: Int64
    var name: String
    var coords: Coords
}

class SystemOperation : Operation {
    let systems: [Int64: System]
    let file: String
    let handle: FileHandle

    init(file: String, systems: [Int64: System], handle: FileHandle) {
        self.file = file
        self.systems = systems
        self.handle = handle
    }

    override func main() {
        if let aStreamReader = StreamReader(path: file, chunkSize: 100 * 1024) {
            defer {
                aStreamReader.close()
            }
            while let rowline = aStreamReader.nextLine() {
                if let systemObj = try? JSONDecoder().decode(SystemObj.self, from: rowline) {
                    OSAtomicIncrement32(&found)
                   if let system = systems[systemObj.id64] {
                        let rowstring = "\(systemObj.name)\t\(systemObj.coords.x)\t\(systemObj.coords.y)\t\(systemObj.coords.z)\t\(system.elw)\t\(system.ww)\t\(system.wwt)\t\(system.aw)\t\(system.tf)\n"
                        if let rowdata = rowstring.data(using: .isoLatin1, allowLossyConversion: false) {
                            synced(handle) {
                                saved += 1
                                handle.write(rowdata)
                            }
                        }
                    }
                }
            }
        }
    }
}