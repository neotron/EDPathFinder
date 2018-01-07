//
// Created by neotron on 2018-01-06.
//

#include "X52ProMFD.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include "LiveJournal.h"
#include "DirectOutputCallbacks.h"
#include "ScanMFDPage.h"

#define MASK(X, Y) (((X)&(Y)) == Y)
#define ROK(X) ((res = X) == S_OK)
const DWORD kScanSummaryPage = 0;
const DWORD kScanDetailsPage = 1;
const DWORD kCommanderDetailsPage = 2;

void X52ProMFD::initializeMFD() {
    HRESULT res;

    createPages();

    qDebug() << "Initializing MDF";
    if(!ROK(DirectOutput_Initialize(L"EDPathFinder"))) {
        qDebug() << "Failed to initialize DirectOutput:" << res;
        return;
    }

    if(!ROK(DirectOutput_RegisterDeviceCallback(DirectOutput::onDeviceChanged, this))) {
        qDebug() << "Failed to register device callback:" << res;
        return;
    }

    if(!ROK(DirectOutput_Enumerate(DirectOutput::onEnumerate, this))) {
        qDebug() << "Failed to enumerate devices:"<<res;
        return;
    }
}

X52ProMFD::X52ProMFD(QObject *parent) : QObject(parent) {
    initializeMFD();
    _mfdUpdateTimer.setSingleShot(true);
    connect(&_mfdUpdateTimer, SIGNAL(timeout()), this, SLOT(updateCurrentPage()));
    connect(LiveJournal::instance(), SIGNAL(onEvent(const JournalFile &, const Event &)),
            this, SLOT(handleEvent(const JournalFile &, const Event &)));
}

X52ProMFD::~X52ProMFD() {
    DirectOutput_Deinitialize();
}

void X52ProMFD::onEnumerate(void *device) {
    qDebug() << "Enumerate device" <<device;
    updateActiveDevice(device);
}

void X52ProMFD::updateActiveDevice(void *device) {
    if(_device) { return; }
    GUID guid = {};
    HRESULT res;
    if(!ROK(DirectOutput_GetDeviceType(device, &guid))) {
        qDebug() << "Failed to get device guid:" << res;
        return;
    }
    if(guid == DeviceType_X52Pro) {
        qDebug() << "Found active device";
        configureOutputDevice(device);
    }
}

void X52ProMFD::configureOutputDevice(void *device) {
    HRESULT res;
    _device = device;
    if(!ROK(DirectOutput_RegisterPageCallback(_device, DirectOutput::onPageChange, this))) {
        qDebug() << "Failed to register page callback:"<<res;
        return;
    }
    if(!ROK(DirectOutput_RegisterSoftButtonCallback(_device, DirectOutput::onSoftButtonChange, this))) {
        qDebug() << "Failed to register soft button callback:"<<res;
        return;
    }
    addDevicePages();
}

void X52ProMFD::onDeviceChanged(void *device, bool isAdded) {
    qDebug() << "Device" <<device<<"was"<<(isAdded?"added":"removed");
    if(!isAdded && device == _device) {
        _device = nullptr;
    } else {
        updateActiveDevice(device);
    }
}

void X52ProMFD::onPageChange(void *device, DWORD page, bool isSetActive) {
    if(isSetActive) {
        _currentPage = page;
        updateCurrentPage();
    }
}

void X52ProMFD::updateCurrentPage() {
    HRESULT res;
    if(!_pages.contains(_currentPage)) {
        return;
    }
    for(DWORD line = 0; line < 3 && line < _pages[_currentPage]->numLines(); line++) {
        auto str = _pages[_currentPage]->textForLine(line);
        if(!ROK(DirectOutput_SetString(_device, _currentPage, line, static_cast<DWORD>(str.length()), str.toStdWString().c_str()))){
            qDebug() << "Failed to set line"<<line<<"on page"<<_currentPage<<"with error:"<<res;
        }
    }
}

void X52ProMFD::onSoftButtonChange(void *device, DWORD buttonMask) {
    if(!_pages.contains(_currentPage)) {
        return;
    }
    bool didPage = false;
    if(MASK(buttonMask, SoftButton_Up)) {
        didPage = _pages[_currentPage]->stepLine(true);
    } else if(MASK(buttonMask, SoftButton_Down)) {
        didPage = _pages[_currentPage]->stepLine(false);
    } else if(MASK(buttonMask, SoftButton_Select)) {
        if(_pages[_currentPage]->scrollWheelclick()) {
            updateCurrentPage();
        }
    }
    if(didPage) {
        updateCurrentPage();
    }
}

void X52ProMFD::addDevicePages() {
    HRESULT res;
    for(auto pageId: _pages.keys()) {
        if(!ROK(DirectOutput_AddPage(_device, pageId, nullptr, 0))) {
            qDebug() << "Failed to add page" << pageId << ":" << res;
        }
    }
}

void X52ProMFD::createPages() {
    _pages[kScanSummaryPage] = new ScanMFDPage(this);
}

void X52ProMFD::handleEvent(const JournalFile &journal, const Event &event) {
    qDebug() << "Handle Event "<<event.obj();
    QList<DWORD> modified;
    switch(event.type()) {
    case EventTypeScan:
        _pages[kScanSummaryPage]->updateWithEvent(event);
        modified.push_back(kScanSummaryPage);
        break;
    default:
        break;
    }
    for(auto page: modified) {
        if(_currentPage == page) {
            _mfdUpdateTimer.start(100);
            break;
        }
    }
}

#endif
