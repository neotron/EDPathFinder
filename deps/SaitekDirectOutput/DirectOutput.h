#ifndef _DIRECTOUTPUT_H_
#define _DIRECTOUTPUT_H_
//=============================================================================
// License
//=============================================================================
//
// Copyright (c) 2008 Saitek
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//=============================================================================
#ifdef __cplusplus
extern "C" {
#include <windows.h>
#endif
//=============================================================================
// Callbacks

typedef void (__stdcall *Pfn_DirectOutput_EnumerateCallback)(void* hDevice, void* pCtxt);
typedef void (__stdcall *Pfn_DirectOutput_DeviceChange)(void* hDevice, bool bAdded, void* pCtxt);
typedef void (__stdcall *Pfn_DirectOutput_PageChange)(void* hDevice, DWORD dwPage, bool bSetActive, void* pCtxt);
typedef void (__stdcall *Pfn_DirectOutput_SoftButtonChange)(void* hDevice, DWORD dwButtons, void* pCtxt);

//=============================================================================
// Error Codes

const HRESULT E_PAGENOTACTIVE = 0xFF040001;
const HRESULT E_BUFFERTOOSMALL = 0xFF040000 | ERROR_BUFFER_OVERFLOW; // 0xFF04006F

//=============================================================================
// Constants

// {29DAD506-F93B-4F20-85FA-1E02C04FAC17}
const GUID DeviceType_X52Pro		= { 0x29DAD506, 0xF93B, 0x4F20, { 0x85, 0xFA, 0x1E, 0x02, 0xC0, 0x4F, 0xAC, 0x17 } };
// {3E083CD8-6A37-4A58-80A8-3D6A2C07513E}
const GUID DeviceType_Fip			= { 0x3E083CD8, 0x6A37, 0x4A58, { 0x80, 0xA8, 0x3D, 0x6A, 0x2C, 0x07, 0x51, 0x3E } };

const DWORD SoftButton_Select = 0x00000001; // X52Pro ScrollClick
const DWORD SoftButton_Up     = 0x00000002; // X52Pro ScrollUp, FIP RightScrollClockwize
const DWORD SoftButton_Down   = 0x00000004; // X52Pro ScrollDown, FIP RightScrollAnticlockwize
const DWORD SoftButton_Left   = 0x00000008; // FIP LeftScrollAnticlockwize
const DWORD SoftButton_Right  = 0x00000010; // FIP LeftScrollClockwize
const DWORD SoftButton_1      = 0x00000020; // FIP LeftButton1
const DWORD SoftButton_2      = 0x00000040; // FIP LeftButton2
const DWORD SoftButton_3      = 0x00000080; // FIP LeftButton3
const DWORD SoftButton_4      = 0x00000100; // FIP LeftButton4
const DWORD SoftButton_5      = 0x00000200; // FIP LeftButton5
const DWORD SoftButton_6      = 0x00000400; // FIP LeftButton6

const DWORD FLAG_SET_AS_ACTIVE = 0x00000001; // Set this page as the Active Page

//=============================================================================
// Structures
typedef struct SRequestStatus
{
	DWORD dwHeaderError;
	DWORD dwHeaderInfo;
	DWORD dwRequestError;
	DWORD dwRequestInfo;
} SRequestStatus, *PSRequestStatus;

//=============================================================================
// Library Initialization

// HRESULT DirectOutput_Initialize(const wchar_t* wszPluginName);
// Initialize the library
// Parameters
//    wszPluginName : null-terminated wchar_t name of the plugin. Used for debugging purposes. Can be NULL
// Returns
//    S_OK : succeeded
HRESULT __stdcall DirectOutput_Initialize(const wchar_t* wszPluginName);

// HRESULT DirectOutput_Deinitialize();
// Cleanup the library
// Parameters (None)
// Returns
//    S_OK : succeeded
HRESULT __stdcall DirectOutput_Deinitialize();

// HRESULT DirectOutput_RegisterDeviceCallback(Pfn_DirectOutput_DeviceChange pfnCb, void* pCtxt);
// Register a callback. Callback will be called whenever a device is added or removed, or when DirectOutput_Enumerate is called
// Parameters
//     pfnCb : Pointer to the callback function to be called when a device is added or removed
//     pCtxt : Caller supplied context pointer, passed to the callback function
// Returns
//     S_OK : succeeded
HRESULT __stdcall DirectOutput_RegisterDeviceCallback(Pfn_DirectOutput_DeviceChange pfnCb, void* pCtxt);

// HRESULT DirectOutput_Enumerate();
// Enumerate all devices currently attached. Calls DeviceChange callback.
// Parameters (None)
// Returns
//     S_OK : succeeded
//HRESULT __stdcall DirectOutput_Enumerate();
HRESULT __stdcall DirectOutput_Enumerate(Pfn_DirectOutput_EnumerateCallback pfnCb, void* pCtxt);

//=============================================================================
// Recieving Notifications From Devices

// HRESULT DirectOutput_RegisterPageCallback(void* hDevice, Pfn_DirectOutput_PageChange pfnCb, void* pCtxt);
// Register a callback. Called when the page changes. Callee will only recieve notifications about pages they added
// Parameters
//     hDevice : opaque device handle
//     pfnCb : caller supplied callback function, called when the active page is changed to/from one of the caller's pages
//     pCtxt : caller supplied context pointer, passed to the callback function
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
HRESULT __stdcall DirectOutput_RegisterPageCallback(void* hDevice, Pfn_DirectOutput_PageChange pfnCb, void* pCtxt);

// HRESULT DirectOutput_RegisterSoftButtonCallback(void* hDevice, Pfn_DirectOutput_SoftButtonChange pfnCb, void* pCtxt);
// Register a callback. Called when the soft buttons are changed and the callee's page is active
// Parameters
//     hDevice : opaque device handle
//     pfnCb : caller supplied callback function, called when the soft buttons are changed and one of the caller's pages is active
//     pCtxt : caller supplied context pointer, passed to the callback function
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
HRESULT __stdcall DirectOutput_RegisterSoftButtonCallback(void* hDevice, Pfn_DirectOutput_SoftButtonChange pfnCb, void* pCtxt);

//=============================================================================
// Query Information From The Device

// HRESULT DirectOutput_GetDeviceType(void* hDevice, LPGUID pGuid);
// Get the device type GUID. See DeviceType_* constants
// Parameters
//     hDevice : opaque device handle
//     pGuid : pointer to GUID to recieve device type identifier. See DeviceType_* constants
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_INVALIDARG : pGuid is NULL
HRESULT __stdcall DirectOutput_GetDeviceType(void* hDevice, LPGUID pGuid);

// HRESULT DirectOutput_GetDeviceInstance(void* hDevice, LPGUID pGuid);
// Get the device instance GUID used by IDirectInput::CreateDevice
// Parameters
//     hDevice : opaque device handle
//     pGuid : pointer to GUID to recieve device's DirectInput Instance Guid.
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_INVALIDARG : pGuid is NULL
//     E_NOTIMPL : hDevice does not support DirectInput.
HRESULT __stdcall DirectOutput_GetDeviceInstance(void* hDevice, LPGUID pGuid);

//=============================================================================
// Profiling Support - X52 ONLY (?)

// HRESULT DirectOutput_SetProfile(void* hDevice, DWORD cchProfile, const wchar_t* wszProfile);
// Set the profile used on the device.
// Parameters
//     hDevice : opaque device handle
//     cchProfile : count of wchar_t's in wszProfile
//     wszProfile : full path of the profile to activate. passing NULL will clear the profile
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not support SST profiles
HRESULT __stdcall DirectOutput_SetProfile(void* hDevice, DWORD cchProfile, const wchar_t* wszProfile);

//=============================================================================
// Page Commands

// HRESULT DirectOutput_AddPage(void* hDevice, DWORD dwPage, DWORD dwFlags)
// Adds a page to the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : caller assigned page id to add
//     wszDebugName : null-terminated wchar_t string. Only used for debugging, can be NULL
//     dwFlags : flags ( 0 | FLAG_SET_AS_ACTIVE )
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
HRESULT __stdcall DirectOutput_AddPage(void* hDevice, DWORD dwPage, const wchar_t* wszDebugName, DWORD dwFlags);

// HRESULT DirectOutput_RemovePage(void* hDevice, DWORD dwPage)
// Removes a page from the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : caller assigned page id to remove
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_INVALIDARG : dwPage is not a valid page id
HRESULT __stdcall DirectOutput_RemovePage(void* hDevice, DWORD dwPage);

//=============================================================================
// Simple Displaying Commands

// HRESULT DirectOutput_SetLed(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwValue);
// Set the state of a LED on the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : page to display the led on
//     dwIndex : index of the led
//     dwValue : value of the led (0 is off)
// returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not have any leds
//     E_INVALIDARG : dwPage or dwIndex is not a valid id
//     E_PAGENOTACTIVE : dwPage is not the active page
HRESULT __stdcall DirectOutput_SetLed(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwValue);

// HRESULT DirectOutput_SetString(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchValue, const wchar_t* wszValue);
// Set the string on the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : page to display the string on
//     dwIndex : index of the string
//     cchValue : the count of wchar_t's in wszValue
//     wszValue : the string to display
// returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not have any strings
//     E_INVALIDARG : dwPage or dwIndex is not a valid id
//     E_PAGENOTACTIVE : dwPage is not the active page
HRESULT __stdcall DirectOutput_SetString(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchValue, const wchar_t* wszValue);

// HRESULT DirectOutput_SetImage(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cbValue, const void* pvValue);
// Set the image on the device.
// Parameters
//     hDevice : opaque device handle
//     dwPage : page to display the image on
//     dwIndex : index of the image
//     cbValue : the count of bytes of pvValue
//     pvValue : the raw bytes from a BMP (only the bytes that contain pixel data - must be correct format and size)
// returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not have any images
//     E_INVALIDARG : dwPage or dwIndex is not a valid id
//     E_PAGENOTACTIVE : dwPage is not the active page
//     E_BUFFERTOOSMALL : cbValue is not of the correct size
HRESULT __stdcall DirectOutput_SetImage(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cbValue, const void* pvValue);

// HRESULT DirectOutput_SetImageFromFile(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchFilename, const wchar_t* wszFilename);
// Set the image on the device from a file.
// Parameters
//     hDevice : opaque device handle
//     dwPage : page to display the image on
//     dwIndex : index of the image
//     cchFilename : the count of wchar_t's in wszFilename
//     wszFilename : the full path to the image file to display. Must be a BMP or JPG file
// returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not have any images
//     E_INVALIDARG : dwPage or dwIndex is not a valid id
//     E_PAGENOTACTIVE : dwPage is not the active page
HRESULT __stdcall DirectOutput_SetImageFromFile(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchFilename, const wchar_t* wszFilename);

//=============================================================================
// Server Side Commands - FIP ONLY

// HRESULT DirectOutput_StartServer(void* hDevice, DWORD cchFilename, const wchar_t* wszFilename, LPDWORD pdwServerId, PSRequestStatus psStatus);
// Download and start a server application on the device
// Parameters
//     hDevice : opaque device handle
//     cchFilename : count of wchar_t's in wszFilename
//     wszFilename : full path to the server application to run
//     pdwServerId : pointer to a DWORD that recieves the server id
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow server applications
//     E_FAIL : fatal error
HRESULT __stdcall DirectOutput_StartServer(void* hDevice, DWORD cchFilename, const wchar_t* wszFilename, LPDWORD pdwServerId, PSRequestStatus psStatus);

// HRESULT DirectOutput_CloseServer(void* hDevice, DWORD dwServerId, PSRequestStatus psStatus);
// Stop and cleanup a server application on the device
// Parameters
//     hDevice : opaque device handle
//     dwServerId : server id returned from DirectOutput_StartServer
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow server applications
//     E_FAIL : fatal error
HRESULT __stdcall DirectOutput_CloseServer(void* hDevice, DWORD dwServerId, PSRequestStatus psStatus);

// HRESULT DirectOutput_SendServerMsg(void* hDevice, DWORD dwServerId, DWORD dwRequest, DWORD dwPage, DWORD cbIn, const void* pvIn, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);
// Send a message to a server application on the device
// Parameters
//     hDevice : opaque device handle
//     dwServerId : server id returned from DirectOutput_StartServer
//     dwRequest : user defined request code
//     dwPage : page id of the message
//     cbIn : count of BYTEs of the input buffer
//     pvIn : input buffer
//     cbOut : count of BYTEs of the output buffer
//     pvOut : output buffer
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow server applications
//     E_FAIL : fatal error
//     E_PAGENOTACTIVE : dwPage is not the active page and the server tried to access the display
HRESULT __stdcall DirectOutput_SendServerMsg(void* hDevice, DWORD dwServerId, DWORD dwRequest, DWORD dwPage, DWORD cbIn, const void* pvIn, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);

// HRESULT DirectOutput_SendServerFile(void* hDevice, DWORD dwServerId, DWORD dwPage, DWORD cbInHdr, const void* pvInHdr, DWORD cchFile, const wchar_t* wszFile, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);
// Send a message to a server application on the device. The file is appended to the user defined header data.
// Parameters
//     hDevice : opaque device handle
//     dwServerId : server id returned from DirectOutput_StartServer
//     dwRequest : user defined request code
//     dwPage : page id of the message
//     cbInHdr : count of BYTEs of the input buffer header
//     pvIn : input buffer header
//     cchFile : count of wchar_t's in the filename
//     wszFile : full path to file. Contents of file are appended to the input header
//     cbOut : count of BYTEs of the output buffer
//     pvOut : output buffer
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow server applications
//     E_FAIL : fatal error
//     E_PAGENOTACTIVE : dwPage is not the active page and the server tried to access the display
HRESULT __stdcall DirectOutput_SendServerFile(void* hDevice, DWORD dwServerId, DWORD dwRequest, DWORD dwPage, DWORD cbInHdr, const void* pvInHdr, DWORD cchFile, const wchar_t* wszFile, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);

//=============================================================================
// File Operations - FIP ONLY

// HRESULT DirectOutput_SaveFile(void* hDevice, DWORD dwPage, DWORD dwFile, DWORD cchFilename, const wchar_t* wszFilename, PSRequestStatus psStatus);
// Save a file on the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : page id of the message
//     dwFile : file id to use for this file
//     cchFilename : count of wchar_t's in wszFilename
//     wszFilename : full path to file to save
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow saving files
//     E_FAIL : fatal error
HRESULT __stdcall DirectOutput_SaveFile(void* hDevice, DWORD dwPage, DWORD dwFile, DWORD cchFilename, const wchar_t* wszFilename, PSRequestStatus psStatus);

// HRESULT DirectOutput_DisplayFile(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwFile, PSRequestStatus psStatus);
// Display a previously saved file on the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : page id of the message
//     dwIndex : index of the output to display on
//     dwFile : file id to use for this file
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow displaying files
//     E_PAGENOTACTIVE : the page is not active
//     E_FAIL : fatal error
HRESULT __stdcall DirectOutput_DisplayFile(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwFile, PSRequestStatus psStatus);

// HRESULT DirectOutput_DeleteFile(void* hDevice, DWORD dwPage, DWORD dwFile, PSRequestStatus psStatus);
// Delete a file from the device
// Parameters
//     hDevice : opaque device handle
//     dwPage : page id of the message
//     dwFile : file id to use for this file
//     psStatus : pointer to obtain additional error details. Optional
// Returns
//     S_OK : succeeded
//     E_HANDLE : hDevice is not a valid device handle
//     E_NOTIMPL : hDevice does not allow deleting files
//     E_FAIL : fatal error
HRESULT __stdcall DirectOutput_DeleteFile(void* hDevice, DWORD dwPage, DWORD dwFile, PSRequestStatus psStatus);

// HRESULT DirectOutput_GetSerialNumber(void* hDevice, wchar_t* pszSerialNumber, DWORD dwSize);
// Get the device unique serial number
// Parameters
//     hDevice : opaque device handle
//     pszSerialNumber : the serial number string
//     dwSize : the number of the serial number string
// Returns
//     S_OK : succeeded
//     E_FAIL : error
HRESULT __stdcall DirectOutput_GetSerialNumber(void* hDevice, wchar_t* pszSerialNumber, DWORD dwSize);

//=============================================================================
// Function Pointers

typedef HRESULT (__stdcall *Pfn_DirectOutput_Initialize)(const wchar_t* wszPluginName);
typedef HRESULT (__stdcall *Pfn_DirectOutput_Deinitialize)();
typedef HRESULT (__stdcall *Pfn_DirectOutput_RegisterDeviceCallback)(Pfn_DirectOutput_DeviceChange pfnCb, void* pCtxt);
//typedef HRESULT (__stdcall *Pfn_DirectOutput_Enumerate)();
typedef HRESULT (__stdcall *Pfn_DirectOutput_Enumerate)(Pfn_DirectOutput_EnumerateCallback pfnCb, void* pCtxt);
typedef HRESULT (__stdcall *Pfn_DirectOutput_RegisterPageCallback)(void* hDevice, Pfn_DirectOutput_PageChange pfnCb, void* pCtxt);
typedef HRESULT (__stdcall *Pfn_DirectOutput_RegisterSoftButtonCallback)(void* hDevice, Pfn_DirectOutput_SoftButtonChange pfnCb, void* pCtxt);
typedef HRESULT (__stdcall *Pfn_DirectOutput_GetDeviceType)(void* hDevice, LPGUID pGuid);
typedef HRESULT (__stdcall *Pfn_DirectOutput_GetDeviceInstance)(void* hDevice, LPGUID pGuid);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SetProfile)(void* hDevice, DWORD cchProfile, const wchar_t* wszProfile);
typedef HRESULT (__stdcall *Pfn_DirectOutput_AddPage)(void* hDevice, DWORD dwPage, DWORD dwFlags);
typedef HRESULT (__stdcall *Pfn_DirectOutput_RemovePage)(void* hDevice, DWORD dwPage);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SetLed)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwValue);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SetString)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchValue, const wchar_t* wszValue);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SetImage)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cbValue, const void* pvValue);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SetImageFromFile)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchFilename, const wchar_t* wszFilename);
typedef HRESULT (__stdcall *Pfn_DirectOutput_StartServer)(void* hDevice, DWORD cchFilename, const wchar_t* wszFilename, LPDWORD pdwServerId, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_CloseServer)(void* hDevice, DWORD dwServerId, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SendServerMsg)(void* hDevice, DWORD dwServerId, DWORD dwRequest, DWORD dwPage, DWORD cbIn, const void* pvIn, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SendServerFile)(void* hDevice, DWORD dwServerId, DWORD dwRequest, DWORD dwPage, DWORD cbInHdr, const void* pvInHdr, DWORD cchFile, const wchar_t* wszFile, DWORD cbOut, void* pvOut, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_SaveFile)(void* hDevice, DWORD dwPage, DWORD dwFile, DWORD cchFilename, const wchar_t* wszFilename, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_DisplayFile)(void* hDevice, DWORD dwPage, DWORD dwFile, DWORD dwIndex, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_DeleteFile)(void* hDevice, DWORD dwPage, DWORD dwFile, PSRequestStatus psStatus);
typedef HRESULT (__stdcall *Pfn_DirectOutput_GetSerialNumber)(void* hDevice, wchar_t* pszSerialNumber, DWORD dwSize);

//=============================================================================
#ifdef __cplusplus
};
#endif
//=============================================================================
#endif

