#define _USE_MATH_DEFINES
#include <math.h>
#include "BearingCalculator.h"
#include "ui_BearingCalculator.h"

BearingCalculator::BearingCalculator(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::BearingCalculator)
{
    _ui->setupUi(this);
}

BearingCalculator::~BearingCalculator()
{
    delete _ui;
}

void BearingCalculator::calculateBearingAndDistance() {
    bool hasLat = false, hasLon = false;
    LatLong start(startCoords(&hasLat));
    LatLong end(endCoords(&hasLon));

    // Check to see if we have valid coordinates.
    if(!(hasLat && hasLon)) {
        _ui->output->setText("Enter coordinates to calculate...");
        return; }

    auto lonStart = start.lon.toRadians();
    auto lonDest = end.lon.toRadians();
    auto latStart = start.lat.toRadians();
    auto latDest = end.lat.toRadians();
    auto deltaLon = lonDest - lonStart;
    auto deltaLat = log(tan(M_PI_4 + latDest/2)/tan(M_PI_4 + latStart/2));
    auto bearing = fmod(Degrees(atan2(deltaLon, deltaLat)).toDegrees() + 360, 360.0);

    QString output = QString("Bearing to destination: %1°").arg(static_cast<int>(bearing));

    bool hasRadius = false;
    auto R = _ui->radius->text().toDouble(&hasRadius);
    double distance = 0.0;
    if(hasRadius && R > 0) {
        deltaLat = (end.lat - start.lat).toRadians();
        deltaLon = (end.lon - start.lon).toRadians();

        auto a = sin(deltaLat / 2) * sin(deltaLat / 2) + cos(latStart) * cos(latDest) * sin(deltaLon / 2) * sin(deltaLon / 2);
        auto c = 2 * atan2(sqrt(a), sqrt(1 - a));
        distance = R * c;
        if(distance > 1) {
            output.append(QString(", Distance: %1 km").arg(static_cast<int>(distance)));
        } else {
            output.append(QString(", Distance: %1 m").arg(static_cast<int>(1000*distance)));
        }
    }
    _ui->output->setText(output);
}

const BearingCalculator::LatLong BearingCalculator::endCoords(bool *ok) {
    return coordsFor(_ui->destLat, _ui->destLon, ok);
}

const BearingCalculator::LatLong BearingCalculator::startCoords(bool *ok) {
    return coordsFor(_ui->sourceLat, _ui->sourceLon, ok);
}

const BearingCalculator::LatLong BearingCalculator::coordsFor(QLineEdit *lat, QLineEdit *lon, bool *ok) {
    bool hasLat = false;
    bool hasLon = false;
    LatLong coords;
    coords.lat.val = lat->text().toDouble(&hasLat);
    coords.lon.val = lon->text().toDouble(&hasLon);
    if(ok) {
        *ok = hasLat && hasLon;
    }
    return coords;
}

void BearingCalculator::setDestination(double lat, double lon, double radius) {
    _ui->destLon->setText(QString("%1").arg(lon));
    _ui->destLat->setText(QString("%1").arg(lat));
    _ui->radius->setText(QString("%1").arg(radius));
}
