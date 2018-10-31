import Foundation
#if os(Linux)
import Glibc
#else
import Darwin
#endif
import Dispatch

struct Body: Decodable {
    var systemId64: Int64
    var subType: String
    var terraformingState: String?
    var earthMasses: Float
}

let queue = OperationQueue()
queue.maxConcurrentOperationCount = 28

//let modQueue = Dispatch.queue()
var valuableSystems = [Int64:System]()
var bodies = 0
var elw = 0
var ww = 0
var tf = 0
var aw = 0
var systems = 0
let lock = NSLock() 
func parseSystem(_ systemLine: Data) {
    if let body = try? JSONDecoder().decode(Body.self, from: systemLine) {
        synced(lock) {
            let existingSystem = valuableSystems[body.systemId64]
            if existingSystem != nil {
                systems += 1
            }
            var isTF = false
            if let tf = body.terraformingState, tf == "Candidate for terraforming" {
                isTF = true
            }
            var system = existingSystem ?? System()
            switch (body.subType) {
            case "Earth-like world":
                system.elw += 1
                elw += 1
                system.value += estimatedWorth(type: .elw, mass: body.earthMasses)
            case "Water world":
                system.ww += 1
                ww += 1
                if isTF {
                    system.wwt += 1
                }
                system.value += estimatedWorth(type: .ww, mass: body.earthMasses, tf: isTF)
            case "Ammonia world":
                system.aw += 1
                aw += 1
                system.value += estimatedWorth(type: .aw, mass: body.earthMasses)
            case "Metal-rich body":
                if isTF {
                    system.tf += 1
                    tf += 1
                }
                system.value += estimatedWorth(type: .mr, mass: body.earthMasses, tf: isTF)
            case "High metal content world":
                if isTF {
                    system.tf += 1
                    tf += 1
                }
                system.value += estimatedWorth(type: .hmc, mass: body.earthMasses, tf: isTF)

            default:
                if isTF {
                    system.tf += 1
                    tf += 1
                    system.value += estimatedWorth(type: .other, mass: body.earthMasses, tf: isTF)
                }
            }
            valuableSystems[body.systemId64] = system
            bodies += 1
            if (bodies % 4997) == 0 {
                print("\rBodies: \(bodies), ELW: \(elw), WW: \(ww), AW: \(aw), TF: \(tf), T$: \(totalValue / 1000000000) Billion.", terminator: "")
                fflush(stdout)
            }
        }
    }
}

//let systemrows = try! String(contentsOfFile: "valuable-bodies.jsonl").components(separatedBy: "\n")
if let aStreamReader = StreamReader(path: "valuable-bodies.jsonl", chunkSize: 1024 * 1024) {
    defer {
        aStreamReader.close()
    }
    while let systemLine = aStreamReader.nextLine() {
        let local = systemLine
        queue.addOperation {
            parseSystem(local)
        }
    }
    queue.waitUntilAllOperationsAreFinished()
    print("\nParsed \(bodies) bodies in \(valuableSystems.count) systems, with an estimated total value of \(totalValue/1000000000) billion credits.")
} else {
    print("Failed to open valuable-bodies.jsonl")
    exit(1)
}

FileManager.default.createFile(atPath: "valuable-systems.csv", contents: nil)
guard let handle = FileHandle(forUpdatingAtPath: "valuable-systems.csv") else {
    print("Failed to open output")
    exit(1)
}

handle.truncateFile(atOffset: 0)
var found: Int32 = 0
var saved: Int = 0

let dir = "../data/tmp"
let files = (try? FileManager.default.contentsOfDirectory(atPath: dir))!
for file in files {
    let op = SystemOperation(file: "\(dir)/\(file)", systems: valuableSystems, handle: handle);
    queue.addOperation(op)
}
while queue.operationCount > 0 {
    print("\rResolving system coords: \(queue.operationCount) / \(saved) / \(found)                  ", terminator: "")
    fflush(stdout)
    Thread.sleep(forTimeInterval: 0.1)
}
print("\nCompleted.")
queue.waitUntilAllOperationsAreFinished()
handle.closeFile()
exit(0)
