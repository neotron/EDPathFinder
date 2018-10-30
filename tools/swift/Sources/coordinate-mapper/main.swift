import Foundation
#if os(Linux)
import Glibc
#else
import Darwin
#endif
import Dispatch

//let modQueue = Dispatch.queue()
var valuableSystems = [Int64:System]()
func loadBodies() {
    //let systemrows = try! String(contentsOfFile: "valuable-bodies.jsonl").components(separatedBy: "\n")
    if let aStreamReader = StreamReader(path: "valuable-bodies.jsonl", chunkSize: 1024 * 1024) {
        defer {
            aStreamReader.close()
        }
        var bodies = 0
        var elw = 0
        var ww = 0
        var tf = 0
        var aw = 0
        var systems = 0
        while let systemline = aStreamReader.nextLine() {
            do {
                let json = try JSONSerialization.jsonObject(with: systemline, options: []) as! NSDictionary
                guard let systemid: Int64 = try json.value(for: "systemId64") else {
                    continue;
                }
                let existingsystem = valuableSystems[systemid]
                if existingsystem != nil {
                    systems += 1
                }

                var system = existingsystem ?? System()
                switch try json.value(for: "subType") as String {
                case "Earth-like world":
                    system.elw += 1
                    elw += 1
                case "Water world":
                    system.ww += 1
                    ww += 1
                    if try json.value(for: "terraformingState") != "Not terraformable" {
                        system.wwt += 1
                    }
                case "Ammonia world":
                    system.aw += 1
                    aw += 1
                default:
                    if try json.value(for: "terraformingState") != "Not terraformable" {
                        system.tf += 1
                        tf += 1
                    }
                }
                valuableSystems[systemid] = system
                bodies += 1
                if (bodies % 5000) == 0 {
                    print("\rBodies: \(bodies), ELW: \(elw), WW: \(ww), AW: \(aw), TF: \(tf)                 ", terminator: "")
                    fflush(stdout)
                }
            } catch {
                continue;
            }
        }
        print("\rparsed \(bodies) bodies in \(valuableSystems.count) systems.")
    }
}
loadBodies()

FileManager.default.createFile(atPath: "valuable-systems.csv", contents: nil)
guard let handle = FileHandle(forUpdatingAtPath: "valuable-systems.csv") else {
    print("Failed to open output")
    exit(1)
}

handle.truncateFile(atOffset: 0)
var found: Int32 = 0
var saved: Int = 0

let queue = OperationQueue()
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
