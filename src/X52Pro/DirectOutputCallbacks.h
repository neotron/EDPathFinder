//
// Created by neotron on 2018-01-06.
//
#pragma once
namespace DirectOutput {
    void onEnumerate(void *hDevice, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onEnumerate(hDevice);
    }

    void onDeviceChanged(void *hDevice, bool bAdded, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onDeviceChanged(hDevice, bAdded);

    }

    void onPageChange(void *hDevice, DWORD dwPage, bool bSetActive, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onPageChange(hDevice, dwPage, bSetActive);
    }

    void onSoftButtonChange(void *hDevice, DWORD dwButtons, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onSoftButtonChange(hDevice, dwButtons);
    }
}
