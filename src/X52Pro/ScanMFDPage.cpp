//
// Created by neotron on 2018-01-06.
//

#include "ScanMFDPage.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include <deps/EDJournalQT/src/Event.h>


//{
//   "timestamp":"2018-01-01T00:24:58Z",
//   "event":"Scan",
//   "BodyName":"Kajuku 2 b",
//   "DistanceFromArrivalLS":1583.549438,
//   "TidalLock":true,
//   "TerraformState":"",
//   "PlanetClass":"Rocky body",
//   "Atmosphere":"",
//   "AtmosphereType":"None",
//   "Volcanism":"minor metallic magma volcanism",
//   "MassEM":0.004306,
//   "Radius":1140004.875000,
//   "SurfaceGravity":1.320512,
//   "SurfaceTemperature":624.271240,
//   "SurfacePressure":0.000000,
//   "Landable":true,
//   "Materials":[
//      {
//         "Name":"iron",
//         "Percent":18.426849
//      },
//   ],
//   "SemiMajorAxis":606692096.000000,
//   "Eccentricity":0.000011,
//   "OrbitalInclination":0.025912,
//   "Periapsis":27.196075,
//   "OrbitalPeriod":112274.054688,
//   "RotationPeriod":112277.226563,
//   "AxialTilt":0.232441
//}
void ScanMFDPage::updateWithEvent(const Event &ev) {
    if(ev.type() != EventTypeScan) {
        return;
    }
    _lastBodyName = ev.string("BodyName");
    qDebug() << "Scan event in " << _lastBodyName;
    _lines.clear();
    _lines.append(displayName());
    
    auto bodyInfo = ev.string("PlanetClass");
    QString line2, line3;
    if(bodyInfo.isEmpty()) {
        bodyInfo = ev.string("StarType");
        auto lum = ev.string("Luminosity");

        auto surfaceTemp = ev.integer("SurfaceTemperature");
        auto temp        = QString("%1K").arg(surfaceTemp > 999999
                                              ? QString::number(surfaceTemp * 1.0, 'g', 3).replace("+", "")
                                              : QString::number(surfaceTemp));
        auto massStr     = QString::number(ev.doubleValue("StellarMass"), 'f', 1);
        auto radius    = ev.doubleValue("Radius") / 695700000.0;
        auto radiusStr = radius < 0.1 ? "0" : QString::number(radius, 'f', 1);
        line3 = QString("%1 M%2 R%3").arg(temp).arg(massStr).arg(radiusStr);
        line2 = mediumBodyType(bodyInfo) + (lum.isEmpty() ? "" : " " + lum);
        if(line2.length() <= 9) {
            line2 = "Star: " + line2;
        }
    } else {
        auto gravity = calculateGravity(ev);
        auto numDigits = 2;
        if(gravity < 1.0) { --numDigits; }
        if(gravity > 99) { --numDigits; }
        auto landable = ev.boolean("Landable");
        if(landable) {
            line3 = "Landable";
        } else {
            line3 = atmosphereShort(ev.string("AtmosphereType"));
        }
        if(line3.isEmpty()) {
            // Gas giant or somethign
            line2 = mediumBodyType(bodyInfo);

            auto massEM  = ev.doubleValue("MassEM");
            auto massStr = massEM < 10 ? QString::number(massEM, 'f', 1) : QString::number(massEM, 'f', 0);
            auto radius      = ev.doubleValue("Radius");
            QString radiusStr;
            if(radius < 1000) {
                radiusStr = QString("%1m").arg(static_cast<int>(radius));
            } else if(radius < 1000000) {
                radiusStr = QString("%1km").arg(static_cast<int>(radius/1000));
            } else if(radius < 1000000000) {
                radiusStr = QString("%1Mm").arg(static_cast<int>(radius/1000000));
            }
            // 1234567890123456
            // 9.99g M172.2 R1.2
            // 11g M155k R74Mm
            // 11g M1535 R74Mm
            line3 = QString("%1g M%2 R%3").arg(static_cast<int>(gravity)).arg(massStr).arg(radiusStr);
        } else {
            line2 = shorthandBodyType(bodyInfo);
            if(!ev.string("TerraformState").isEmpty()) {
                line2 += " TF";
            }
            line2 += ", "+QString::number(gravity, 'f', numDigits)+"g";
        }
    }
    _lines.append(line2);
    _lines.append(line3);
    qDebug() << "Updated summary lines to"<<endl <<_lines;
}

QString ScanMFDPage::displayName() const {
    if(!_showLongName && _lastBodyName.length() > 16) {
        return "<"+_lastBodyName.right(15);
    } else {
        return _lastBodyName;
    }
}

// Atmosphere in 16 or under letters.
QString ScanMFDPage::atmosphereShort(const QString &atm) {
    QMap<QString, QString> l;
    l["Ammonia"]           = "Ammonia";
    l["AmmoniaOxygen"]     = "Ammonia O2";
    l["AmmoniaRich"]       = "Ammonia Rich";
    l["Argon"]             = "Argon";
    l["ArgonRich"]         = "Argon Rich";
    l["CarbonDioxide"]     = "Carbon Dioxide";
    l["CarbonDioxideRich"] = "CO2 Rich";
    l["EarthLike"]         = "Earth Like";
    l["Helium"]            = "Helium";
    l["Methane"]           = "Methane";
    l["MethaneRich"]       = "Methane Rich";
    l["Neon"]              = "Neon";
    l["NeonRich"]          = "Neon Rich";
    l["Nitrogen"]          = "Nitrogen";
    l["None"]              = "None";
    l["Oxygen"]            = "Oxygen";
    l["SilicateVapour"]    = "Silicate Vapour";
    l["SulphurDioxide"]    = "Sulphur Dioxide";
    l["Water"]             = "Water";
    l["WaterRich"]         = "Water Rich";
    return l.contains(atm) ? l[atm] : atm;
}

// If we scan without a detailed scanner, we don't get gravity
// We still get mass and radius however, so we can still claculate this.
double ScanMFDPage::calculateGravity(const Event &ev) {
    auto planetRadius = ev.doubleValue("Radius");
    auto planetMass = ev.doubleValue("MassEM");
    auto G = 6.67408e-11;
    auto earthMass = 5.9721986e24;
    return (G * planetMass * earthMass / (planetRadius * planetRadius))/9.807;
}

// This maps the details planet type to a shorthand for the summary display.
QString ScanMFDPage::shorthandBodyType(const QString &bodyType) {
    QMap<QString, QString> b;
    b["metal rich body"]                   = "MR";
    b["high metal content body"]           = "HMC";
    b["rocky body"]                        = "Rocky";
    b["icy body"]                          = "Icy";
    b["rocky ice body"]                    = "RoIcy";
    b["earthlike body"]                    = "ELW";
    b["water world"]                       = "WW";
    b["ammonia world"]                     = "AW";
    b["water giant"]                       = "WaGi";
    b["water giant with life"]             = "WaGiL";
    b["gas giant with water based life"]   = "GG WL";
    b["gas giant with ammonia based life"] = "GG AL";
    b["sudarsky class i gas giant"]        = "GG I";
    b["sudarsky class ii gas giant"]       = "GG II";
    b["sudarsky class iii gas giant"]      = "GG III";
    b["sudarsky class iv gas giant"]       = "GG IV";
    b["sudarsky class v gas giant"]        = "GG V";
    b["helium rich gas giant"]             = "HR GG";
    b["supermassiveblackhole"]             = "SM BH";
    b["a_bluewhitesupergiant"]             = "A SG";
    b["f_whitesupergiant"]                 = "F SG";
    b["m_redsupergiant"]                   = "M SG";
    b["m_redgiant"]                        = "M RG";
    b["k_orangegiant"]                     = "K OG";
    b["rogueplanet"]                       = "RP";
    b["nebula"]                            = "NB";
    b["stellarremnantnebula"]              = "SRNB";
    auto lower = bodyType.toLower();
    return b.contains(lower) ? b[lower] : bodyType;
}

// This is a somewhat longer shorthand, but below 16 characters
QString ScanMFDPage::mediumBodyType(const QString &bodyType) {
    QMap<QString, QString> b;
    b["metal rich body"]                   = "Metal Rich";
    b["high metal content body"]           = "High Metal Cont.";
    b["rocky body"]                        = "Rocky";
    b["icy body"]                          = "Icy";
    b["rocky ice body"]                    = "Rocky Icy";
    b["earthlike body"]                    = "Earth Like";
    b["water world"]                       = "Water World";
    b["ammonia world"]                     = "Ammonia World";
    b["water giant"]                       = "Water Giant";
    b["water giant with life"]             = "Water Giant Life";
    b["gas giant with water based life"]   = "GG, Water Life";
    b["gas giant with ammonia based life"] = "GG, Ammonia Life";
    b["sudarsky class i gas giant"]        = "Class I G. Giant";
    b["sudarsky class ii gas giant"]       = "Class II G.Giant";
    b["sudarsky class iii gas giant"]      = "Class III GGiant";
    b["sudarsky class iv gas giant"]       = "Class IV G.Giant";
    b["sudarsky class v gas giant"]        = "Class V G. Giant";
    b["helium rich gas giant"]             = "Helium Rich GG";
    b["supermassiveblackhole"]             = "Super Mas Blk Hl";
    b["a_bluewhitesupergiant"]             = "A Super G.";
    b["f_whitesupergiant"]                 = "F Super G.";
    b["m_redsupergiant"]                   = "Red Super G.";
    b["m_redgiant"]                        = "Red Giant";
    b["k_orangegiant"]                     = "Orange G.";
    b["rogueplanet"]                       = "Rogue Planet";
    b["nebula"]                            = "Nebula";
    b["stellarremnantnebula"]              = "Stellar Remnant";
    auto lower = bodyType.toLower();
    return b.contains(lower) ? b[lower] : bodyType;
}

ScanMFDPage::ScanMFDPage(QObject *parent) : MFDPage(parent) {
    _lines.append(QString("Scan Summary\n\nPending Scan").split("\n"));
}

bool ScanMFDPage::scrollWheelclick() {
    _showLongName = !_showLongName;
    if(!_lines.isEmpty()) {
        _lines[0] = displayName();
        return true;
    }
    return false;
}

#endif
