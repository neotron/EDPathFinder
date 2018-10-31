//
// Created by David Hedbor on 2018-10-29.
//

import Foundation
enum Type {
    case elw, ww, aw, mr, other, hmc
}
// Planet base values
let kValueMetalRich = 52292
let kValueHMC_GGII = 23168
let kValueELW_WW = 155581
let kValueAmmoniaWorld = 232619
let kValueGGI = 3974
let kValueDefaultPlanet = 720
// Planet terraformable bonus
let KBonusHMC = 241607
let kBonusELW_WW = 279088
let kBonusDefaultPlanet = 233971
// Stars
var totalValue:Int64 = 0
func adjustedPlanetValue(_ value: Int, mass: Float) -> Int {
    return Int(Float(value) + (3 * Float(value) * pow(mass, 0.199977) / 5.3))
}
func estimatedWorth(type: Type, mass: Float, tf: Bool = true) -> Int {
    var value = 0
    var bonus = 0
    switch(type) {
    case .mr:
        value = kValueMetalRich
        if tf {
            bonus = KBonusHMC
        }
    case .hmc:
        value = kValueHMC_GGII
        if tf {
            bonus = KBonusHMC
        }
    case .elw:
        value = kValueELW_WW
        bonus = kBonusELW_WW
    case .ww:
        value = kValueELW_WW
        if tf {
            bonus = kBonusELW_WW
        }
    case .aw:
        value = kValueAmmoniaWorld
        break
    case .other:
        value = kValueDefaultPlanet
        bonus = kBonusDefaultPlanet
    }
    let adjustedMass = mass <= 0.0 ? 1.0 : mass
    value = adjustedPlanetValue(value + bonus, mass: adjustedMass)
    totalValue += Int64(value)
    return value
}