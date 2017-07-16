//
// Created by David Hedbor on 2017-07-16.
//

import Foundation

struct System {
    var elw = 0
    var ww = 0
    var wwt = 0
    var tf = 0
    var aw = 0
}

func synced(_ lock: Any, closure: () -> ()) {
    objc_sync_enter(lock)
    closure()
    objc_sync_exit(lock)
}

extension Int  {
    static func seq(_ seq: Data.SubSequence) -> Int? {
        let len = seq.endIndex - seq.startIndex
        let bytesCopy = UnsafeMutablePointer<UInt8>.allocate(capacity: len + 1)
        seq.base.copyBytes(to: bytesCopy, from: seq.startIndex..<seq.endIndex)
        bytesCopy[len] = 0
        var value: Int?
        bytesCopy.withMemoryRebound(to: Int8.self, capacity: len+1) {
            value = atol($0)
        }
        return value;
    }
}

extension Double  {
    static func seq(_ seq: Data.SubSequence) -> Double {
        let len = seq.endIndex - seq.startIndex
        let bytesCopy = UnsafeMutablePointer<UInt8>.allocate(capacity: len + 1)
        seq.base.copyBytes(to: bytesCopy, from: seq.startIndex..<seq.endIndex)
        bytesCopy[len] = 0
        var value: Double = 0.0
        bytesCopy.withMemoryRebound(to: Int8.self, capacity: len+1) {
            value = atof($0)
        }
        return value
    }
}

extension String {
    init?(seq: Data.SubSequence) {
        var start = seq.startIndex
        var end = seq.endIndex

        let len = end - start
        if len > 2 {
            start += 1
            end -= 1
        }
        let data = seq.base.subdata(in: start..<end)
        self.init(data: data, encoding: .utf8)
    }
}
