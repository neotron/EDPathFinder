//
// Created by neotron on 2018-01-06.
//
#pragma once
#include <QMap>
#ifdef Q_OS_WIN
#include <QTimer>
#include "MFDPage.h"
#include "deps/SaitekDirectOutput/DirectOutput.h"
class JournalFile;
class Event;

class X52ProMFD: public QObject {
    Q_OBJECT
public:
    explicit X52ProMFD(QObject *parent = nullptr);

    ~X52ProMFD() override;

    // callbacks from the library
    void __stdcall onDeviceChanged(void *device, bool isAdded);
    void onEnumerate(void *device);
    void onPageChange(void *device, DWORD page, bool isSetActive);
    void onSoftButtonChange(void *device, DWORD buttonMask);

private slots:
    void handleEvent(const JournalFile &journal, const Event &event);
    void updateCurrentPage();

private:
    void initializeMFD();

    void updateActiveDevice(void *device);

    void configureOutputDevice(void *device);

    void addDevicePages();


    void createPages();

    void *_device{};
    QMap<DWORD,MFDPage*> _pages{};
    DWORD _currentPage = -1;
    QTimer _mfdUpdateTimer;
};


#endif
