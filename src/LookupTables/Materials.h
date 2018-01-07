//
// Created by neotron on 2018-01-07.
//


#pragma once

#include <QString>
#include <QMap>
#include <QObject>

class Material {
public:
    enum Rarity {
        VeryCommon, Common, Standard, Rare, VeryRare
    };
    enum Type {
        Manufactured, Encoded, Raw, Invalid
    };

    Material(): _type(Invalid) {}
    Material(QString id, QString name, QString abbreviation, Rarity rarity, Type type);

    const QString &id() const;

    const QString &name() const;

    const QString &abbreviation() const;

    QString rarityName() const;

    Rarity rarity() const;

    Type type() const;

    QString typeName() const;

    bool isValid() const;
private:
    QString _id; // Journal identifier
    QString _name; // Long name
    QString _abbreviation; // Abbreviation
    Rarity  _rarity;
    Type _type;

};

class Materials: public QObject {
    Q_OBJECT
public:
    static Material material(const QString &id);
private:
    static QMap<QString,Material> _materialTable;
};

