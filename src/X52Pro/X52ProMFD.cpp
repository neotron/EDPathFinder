//
// Created by neotron on 2018-01-06.
//

#include "X52ProMFD.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include "LiveJournal.h"
#include "DirectOutputCallbacks.h"
#include "ScanMFDPage.h"
#include "MaterialLogMFDPage.h"

#define MASK(X, Y) (((X)&(Y)) == Y)
#define ROK(X) ((res = X) == S_OK)
const DWORD kScanSummaryPage = 0;
const DWORD kMaterialLogPage = 1;
const DWORD kHelpPage = 2;

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
    for(DWORD line = 0; line < 3; line++) {
        auto str = line < _pages[_currentPage]->numLines()
                   ? _pages[_currentPage]->textForLine(line)
                   : "";
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
        auto name = QString("%1").arg(pageId).toStdWString().c_str();
        if(!ROK(DirectOutput_AddPage(_device, pageId, name, pageId == kHelpPage ? FLAG_SET_AS_ACTIVE : 0))) {
            qDebug() << "Failed to add page" << pageId << ":" << res;
        }
    }
    _currentPage = kHelpPage;
    updateCurrentPage();
}

void X52ProMFD::createPages() {
    _pages[kScanSummaryPage] = new ScanMFDPage(this);
    _pages[kMaterialLogPage] = new MaterialLogMFDPage(this);

    QFile help(":/MFDHelp.txt");
    if(help.open(QIODevice::ReadOnly | QIODevice::Text)) {
        auto helpPage = new MFDPage(this);
        QStringList lines(QString(help.readAll()).split("\n"));
        helpPage->setLines(lines);
        _pages[kHelpPage] = helpPage;
    }
}

void X52ProMFD::handleEvent(const JournalFile &journal, const Event &event) {
    qDebug() << "Handle Event "<<event.obj();
    QSet<DWORD> modified;
    for(DWORD page: _pages.keys()) {
        if(_pages[page]->update(journal, event)) {
            modified.insert(page);
        }
    }
    if(modified.contains(_currentPage)) {
        _mfdUpdateTimer.start(100);
    }
}

#endif
