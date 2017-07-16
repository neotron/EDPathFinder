//
// Created by David Hedbor on 2017-07-16.
//

import Foundation

class SystemOperation : Operation {
    let systems: [Int: System]
    let elws: Set<String>
    let file: String
    let handle: FileHandle

    init(file: String, systems: [Int: System], elws: Set<String>, handle: FileHandle) {
        self.file = file
        self.systems = systems
        self.elws = elws
        self.handle = handle
    }

    override func main() {
        if let aStreamReader = StreamReader(path: file, chunkSize: 100 * 1024) {
            defer {
                aStreamReader.close()
            }
            while let rowline = aStreamReader.nextLine() {
                let row = rowline.split(separator: 44, omittingEmptySubsequences: false)
                if (row.count < 8) {
                    continue
                }
                guard let id = Int.seq(row[0]), Int.seq(row[7]) == 0 else {
                    continue
                }
                guard let systemName = String(seq: row[2]) else {
                    continue
                }
                OSAtomicIncrement32(&found)
                let x = Double.seq(row[3])
                let y = Double.seq(row[4])
                let z = Double.seq(row[5])

                if let system = systems[id] {
                    let rowstring = "\(systemName)\t\(x)\t\(y)\t\(z)\t\(system.elw)\t\(system.ww)\t\(system.wwt)\t\(system.aw)\t\(system.tf)\n"
                    if let rowdata = rowstring.data(using: .isoLatin1, allowLossyConversion: false) {
                        synced(handle) {
                            handle.write(rowdata)
                        }
                    }
                } else if (elws.contains(systemName.uppercased())) {
                    let rowstring = "\(systemName)\t\(x)\t\(y)\t\(z)\t1\t0\t0\t0\t0\n"
                    if let rowdata = rowstring.data(using: .isoLatin1, allowLossyConversion: false) {
                        synced(handle) {
                            handle.write(rowdata)
                        }
                    }
                }
            }
        }
    }
}
