
//
// Created by neotron on 2018-01-07.
//

#include "HabitableZone.h"
#include <QDebug>
const double bolSol = 4.72;
const double pogsonsRatio = -2.5;
const double auInLS = 499.005;
const double innerStellarFlux = 1.1;
const double outerStellarFlux = 0.53;

QMap<QString, QList<BCEntry>> HabitableZone::s_lookup = {
    {"B", {
              {30000, -3.16},
              {22000, -2.35},
              {18700, -1.94},
              {15400, -1.46},
              {13000, -1.02},
              {11900, -0.8}
          }
    },
    {"A", {
              {9520,  -0.3},
              {8200,  -0.15}
          }
    },
    {"F", {
              {7200,  -0.09},
              {6440,  -0.14}
          }
    },
    {"G", {
              {6030,  -0.18},
              {5860,  -0.2},
              {5770,  -0.21}
          },
    },
    {
     "K", {
              {5250,  -0.31},
              {4350,  -0.72}
          }
    },
    {
     "M", {
              {3850,  -1.28},
              {3250,  -2.73},
              {2640,  -4.1}
          }
    }
};

HabZone HabitableZone::habitableZone(const QString &starClass, int temp, double absoluteMagnitude) {
    HabZone zone = {-1, -1};
    auto key = starClass.left(1).toUpper();
    if(!s_lookup.contains(key)) {
        return zone;
    }
    auto bcLookup = s_lookup[key];
    double BC;
    if(temp > bcLookup.first().K) {
        BC = bcLookup.first().BC; // Hotter than hottest
    } else if(temp < bcLookup.last().K) {
        BC = bcLookup.last().BC; // Cooler than coolest
    } else {
        // It's inside the range, so let's find the right spot.
        BCEntry last = {0, 0};
        BCEntry match = {0, 0};
        for(const auto &bc: bcLookup) {
            if(temp > bc.K) {
                match = bc;
                break;
            }
            last = bc; // Keep the one above the match for reference.
        }
        // Temp range of the two BC steps
        double bcDeltaK = last.K - match.K;
        // Where in the range the star temp is
        double starDeltaK = temp - match.K;

        double percentage = starDeltaK / bcDeltaK; // How far in the range we are.
        BC = percentage * (last.BC - match.BC) + match.BC;
    }

    // We now got the approximate Bolometric Correction constant (BC). Let's estimate the habitable zone.
    double bolStar = absoluteMagnitude + BC;
    double absLuminosity = pow(10, (bolStar - bolSol)/pogsonsRatio);

    zone.inner = static_cast<int>(sqrt(absLuminosity / innerStellarFlux) * auInLS);
    zone.outer = static_cast<int>(sqrt(absLuminosity / outerStellarFlux) * auInLS);
    //qDebug() << BC << zone.inner << zone.outer<<bolStar<<absLuminosity;
    return zone;
}

