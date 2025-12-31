// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcHAL.cpp
//   Purpose : Defines constants for max input devices, max axes, max buttons, max POVs, max sliders, max force feedback axes, max force feedback effects, and max force feedback axes per effect
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

int bDirectInputKeyMap[BDIRECTINPUTKEYMAPSIZE] =
{
    DIK_ESCAPE,     // 0   (Escape)
    DIK_1,          // 1   (1)
    DIK_2,          // 2   (2)
    DIK_3,          // 3   (3)
    DIK_4,          // 4   (4)
    DIK_5,          // 5   (5)
    DIK_6,          // 6   (6)
    DIK_7,          // 7   (7)
    DIK_8,          // 8   (8)
    DIK_9,          // 9   (9)
    DIK_0,          // 10  (0)
    DIK_MINUS,      // 11  (-)
    DIK_EQUALS,     // 12  (=)
    DIK_BACK,       // 13  (Backspace)
    DIK_TAB,        // 14  (Tab)
    DIK_A,          // 15  (A)
    DIK_B,          // 16  (B)
    DIK_C,          // 17  (C)
    DIK_D,          // 18  (D)
    DIK_E,          // 19  (E)
    DIK_F,          // 20  (F)
    DIK_G,          // 21  (G)
    DIK_H,          // 22  (H)
    DIK_I,          // 23  (I)
    DIK_J,          // 24  (J)
    DIK_K,          // 25  (K)
    DIK_L,          // 26  (L)
    DIK_M,          // 27  (M)
    DIK_N,          // 28  (N)
    DIK_O,          // 29  (O)
    DIK_P,          // 30  (P)
    DIK_Q,          // 31  (Q)
    DIK_R,          // 32  (R)
    DIK_S,          // 33  (S)
    DIK_T,          // 34  (T)
    DIK_U,          // 35  (U)
    DIK_V,          // 36  (V)
    DIK_W,          // 37  (W)
    DIK_X,          // 38  (X)
    DIK_Y,          // 39  (Y)
    DIK_Z,          // 40  (Z)
    DIK_LBRACKET,   // 41  ([)
    DIK_RBRACKET,   // 42  (])
    DIK_RETURN,     // 43  (Enter)
    DIK_SEMICOLON,  // 44  (;)
    DIK_APOSTROPHE, // 45  (')
    DIK_BACKSLASH,  // 46  (\)
    DIK_GRAVE,      // 47  (`)
    DIK_LCONTROL,   // 48  (Left Ctrl)
    DIK_RCONTROL,   // 49  (Right Ctrl)
    DIK_LSHIFT,     // 50  (Left Shift)
    DIK_RSHIFT,     // 51  (Right Shift)
    DIK_LMENU,      // 52  (Left Alt)
    DIK_RMENU,      // 53  (Right Alt)
    DIK_COMMA,      // 54  (,)
    DIK_PERIOD,     // 55  (.)
    DIK_SLASH,      // 56  (/)
    DIK_SPACE,      // 57  (Space)
    DIK_F1,         // 58  (F1)
    DIK_F2,         // 59  (F2)
    DIK_F3,         // 60  (F3)
    DIK_F4,         // 61  (F4)
    DIK_F5,         // 62  (F5)
    DIK_F6,         // 63  (F6)
    DIK_F7,         // 64  (F7)
    DIK_F8,         // 65  (F8)
    DIK_F9,         // 66  (F9)
    DIK_F10,        // 67  (F10)
    DIK_F11,        // 68  (F11)
    DIK_F12,        // 69  (F12)
    DIK_F13,        // 70  (F13)
    DIK_F14,        // 71  (F14)
    DIK_F15,        // 72  (F15)
    DIK_NUMLOCK,    // 73  (Num Lock)
    DIK_SCROLL,     // 74  (Scroll Lock)
    DIK_CAPITAL,    // 75  (Caps Lock)
    DIK_NUMPAD0,    // 76  (Numpad 0)
    DIK_NUMPAD1,    // 77  (Numpad 1)
    DIK_NUMPAD2,    // 78  (Numpad 2)
    DIK_NUMPAD3,    // 79  (Numpad 3)
    DIK_NUMPAD4,    // 80  (Numpad 4)
    DIK_NUMPAD5,    // 81  (Numpad 5)
    DIK_NUMPAD6,    // 82  (Numpad 6)
    DIK_NUMPAD7,    // 83  (Numpad 7)
    DIK_NUMPAD8,    // 84  (Numpad 8)
    DIK_NUMPAD9,    // 85  (Numpad 9)
    DIK_DECIMAL,    // 86  (Numpad .)
    DIK_MULTIPLY,   // 87  (Numpad *)
    DIK_SUBTRACT,   // 88  (Numpad -)
    DIK_ADD,        // 89  (Numpad +)
    DIK_DIVIDE,     // 90  (Numpad /)
    DIK_NUMPADENTER,// 91  (Numpad Enter)
    DIK_SYSRQ,      // 92  (Print Screen / SysRq)
    DIK_PAUSE,      // 93  (Pause / Break)
    DIK_HOME,       // 94  (Home)
    DIK_END,        // 95  (End)
    DIK_PRIOR,      // 96  (Page Up)
    DIK_NEXT,       // 97  (Page Down)
    DIK_INSERT,     // 98  (Insert)
    DIK_DELETE,     // 99  (Delete)
    DIK_UP,         // 100 (Arrow Up)
    DIK_DOWN,       // 101 (Arrow Down)
    DIK_LEFT,       // 102 (Arrow Left)
    DIK_RIGHT,      // 103 (Arrow Right)
    DIK_LWIN,       // 104 (Left Windows)
    DIK_RWIN,       // 105 (Right Windows)
    DIK_APPS        // 106 (Context Menu)
};

GUID *bJoystickInstanceGuid[BMAXPADS] = { NULL, NULL, NULL, NULL };
TBInputInfo	bInputInfo;

// ********************************************************************************
// Locals

int   bMouseExclusiveMode		= 1;
int   bKeyExclusiveMode		    = 1;
int   bInputEnumAdditionalDebug = 1;
DWORD bLastMouseFilterTime      = 0;
int   bInputSuspended           = 0;

// ********************************************************************************
// Local Functions

void GetAxisRange(TBDeviceInfo *deviceInfo, int axisIndex, const DIDEVICEOBJECTINSTANCEA *lpddoi)
{
    HRESULT      hr;
    DIPROPRANGE  dipRange;

	dipRange.diph.dwObj        = lpddoi->dwType;
    dipRange.diph.dwSize       = sizeof(DIPROPRANGE);
    dipRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipRange.diph.dwHow        = DIPH_BYID;

    hr = deviceInfo->device->GetProperty(DIPROP_RANGE, &dipRange.diph);
    if (FAILED(hr))
    {
        const char *errorStr = DXGetErrorString8A(hr);
        bkPrintf("GetAxisRange: Could not retrieve axis range for axis %d '%s'\n", axisIndex, errorStr);

        deviceInfo->axisMin[axisIndex] = 0;
        deviceInfo->axisMax[axisIndex] = 0xFFFF;
    }
    else
    {
        deviceInfo->axisMin[axisIndex] = dipRange.lMin;
        deviceInfo->axisMax[axisIndex] = dipRange.lMax;
    }

    LONG center    = (deviceInfo->axisMin[axisIndex] + deviceInfo->axisMax[axisIndex]) / 2;
    LONG halfRange = deviceInfo->axisMax[axisIndex] - center;

    deviceInfo->axisHalfRange[axisIndex] = halfRange;
	deviceInfo->axisCenter[axisIndex]    = center;
    deviceInfo->axisScaled[axisIndex]    = (halfRange * 0x23) / 100;
}

// yes, __stdcall IS needed!
int __stdcall EnumDeviceObjectsCallback(const DIDEVICEOBJECTINSTANCEA *lpddoi, void *pvRef)
{
    TBDeviceInfo *deviceInfo = (TBDeviceInfo *)pvRef;
    const GUID &type = lpddoi->guidType;

    uint32 maskBit = 0;
    int axisIndex  = 0;

    if (IsEqualGUID(type, GUID_XAxis)) {
        maskBit   = 0x01;
        axisIndex = 0;
    }
    else if (IsEqualGUID(type, GUID_YAxis)) {
        maskBit   = 0x02;
        axisIndex = 1;
    }
    else if (IsEqualGUID(type, GUID_ZAxis)) {
        maskBit   = 0x04;
        axisIndex = 2;
    }
    else if (IsEqualGUID(type, GUID_RxAxis)) {
        maskBit   = 0x08;
        axisIndex = 3;
    }
    else if (IsEqualGUID(type, GUID_RyAxis)) {
        maskBit   = 0x10;
        axisIndex = 4;
    }
    else if (IsEqualGUID(type, GUID_RzAxis)) {
        maskBit   = 0x20;
        axisIndex = 5;
    }
    else {
        // Unknown object type – just continue enumeration without touching anything
        return DIENUM_CONTINUE;
    }

    deviceInfo->axisPresentMask |= maskBit;
    GetAxisRange(deviceInfo, axisIndex, lpddoi);

    return DIENUM_CONTINUE;
}

// yes, __stdcall IS needed!
int __stdcall EnumDevicesCallback(const DIDEVICEINSTANCEA *lpddi, void *pvRef)
{
    BYTE devType = LOBYTE(lpddi->dwDevType);
    switch (devType) {
    case DI8DEVTYPE_MOUSE:
        // If mouse already handled, skip
        if (bInputInfo.devices[5].flags & 0x1) {
            break;
        }
        if (bInputEnumAdditionalDebug) {
            bkPrintf("EnumDevicesCallback: Found MOUSE input device '%s' '%s'\n", lpddi->tszInstanceName, lpddi->tszProductName);
        }
        // Store device GUID
        bInputInfo.devices[5].guid = lpddi->guidInstance;
        // Create the device
        {
            HRESULT hr = bInputInfo.pDI->CreateDevice(lpddi->guidInstance, &bInputInfo.devices[5].device, NULL);
            if (FAILED(hr)) {
                if (bInputEnumAdditionalDebug) {
                    bkPrintf("EnumDevicesCallback: Could not create MOUSE input device\n");
                }
                return DIENUM_CONTINUE;
            }
            // Set data format for mouse
            hr = bInputInfo.devices[5].device->SetDataFormat(&c_dfDIMouse);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set MOUSE data format '%s'\n", errStr);
                if (bInputInfo.devices[5].device) {
                    // Release device interface completely
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[5].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[5].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            // Set cooperative level (exclusive/non-exclusive + foreground)
            hr = bInputInfo.devices[5].device->SetCooperativeLevel(bMainWindow, (bMouseExclusiveMode | DISCL_FOREGROUND));
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set MOUSE cooperative level '%s'\n", errStr);
                if (bInputInfo.devices[5].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[5].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[5].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            // Retrieve device capabilities
            DIDEVCAPS caps;
            ZeroMemory(&caps, sizeof(caps));
            caps.dwSize = sizeof(DIDEVCAPS);
            hr = bInputInfo.devices[5].device->GetCapabilities(&caps);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not get capabilities for MOUSE device '%s'\n", errStr);
                if (bInputInfo.devices[5].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[5].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[5].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            // Update flags (set bit 1 if device is polled)
            if (caps.dwFlags & (DIDC_POLLEDDEVICE | DIDC_POLLEDDATAFORMAT)) {
                bInputInfo.devices[5].flags |= 0x2;
            }
            // Save number of controls
            bInputInfo.devices[5].numButtons = caps.dwButtons;
            bInputInfo.devices[5].numAxes   = caps.dwAxes;
            // Acquire the device (mouse)
            hr = bInputInfo.devices[5].device->Acquire();
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: *** WARNING *** Could not acquire MOUSE device '%s'\n", errStr);
            }
            // Mark device as initialized
            bInputInfo.devices[5].flags |= 0x1;
        }
        return DIENUM_CONTINUE;

    case DI8DEVTYPE_KEYBOARD:
        if (bInputInfo.devices[4].flags & 0x1) {
            break;
        }
        if (bInputEnumAdditionalDebug) {
            bkPrintf("EnumDevicesCallback: Found KEYBOARD input device '%s' '%s'\n", lpddi->tszInstanceName, lpddi->tszProductName);
        }
        bInputInfo.devices[4].guid = lpddi->guidInstance;
        {
            HRESULT hr = bInputInfo.pDI->CreateDevice(lpddi->guidInstance, &bInputInfo.devices[4].device, NULL);
            if (FAILED(hr)) {
                if (bInputEnumAdditionalDebug) {
                    bkPrintf("EnumDevicesCallback: Could not create KEYBOARD input device\n");
                }
                return DIENUM_CONTINUE;
            }
            hr = bInputInfo.devices[4].device->SetDataFormat(&c_dfDIKeyboard);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set KEYBOARD data format '%s'\n", errStr);
                if (bInputInfo.devices[4].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[4].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[4].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            hr = bInputInfo.devices[4].device->SetCooperativeLevel(bMainWindow, (bKeyExclusiveMode | DISCL_FOREGROUND));
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set KEYBOARD cooperative level '%s'\n", errStr);
                if (bInputInfo.devices[4].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[4].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[4].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            DIDEVCAPS caps;
            ZeroMemory(&caps, sizeof(caps));
            caps.dwSize = sizeof(DIDEVCAPS);
            hr = bInputInfo.devices[4].device->GetCapabilities(&caps);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not get capabilities for KEYBOARD device '%s'\n", errStr);
                if (bInputInfo.devices[4].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[4].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[4].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            if (caps.dwFlags & (DIDC_POLLEDDEVICE | DIDC_POLLEDDATAFORMAT)) {
                bInputInfo.devices[4].flags |= 0x2;
            }
            bInputInfo.devices[4].numButtons = caps.dwButtons;
            hr = bInputInfo.devices[4].device->Acquire();
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: *** WARNING *** Could not acquire KEYBOARD device '%s'\n", errStr);
            }
            bInputInfo.devices[4].flags |= 0x1;
        }
        return DIENUM_CONTINUE;

    case DI8DEVTYPE_JOYSTICK:
    case DI8DEVTYPE_GAMEPAD:
        if (bInputInfo.joystickCount == 4) {
            break;
        }
        if (bInputEnumAdditionalDebug) {
            bkPrintf("EnumDevicesCallback: Found JOYSTICK/GAMEPAD input device '%s' '%s'\n", lpddi->tszInstanceName, lpddi->tszProductName);
        }
        {
            int idxUse = -1;
            // Check if this device was seen before (match GUID)
            for (int i = 0; i < 4; ++i) {
                if (bJoystickInstanceGuid[i] != NULL) {
                    if (IsEqualGUID(*bJoystickInstanceGuid[i], lpddi->guidInstance)) {
                        idxUse = i;
                        break;
                    }
                }
            }
            if (idxUse != -1) {
                bkPrintf("EnumDevicesCallback: Matching requested device (%d)\n", idxUse);
            } else {
                for (int j = 0; j < 4; ++j) {
                    if (bJoystickInstanceGuid[j] == NULL && !(bInputInfo.devices[j].flags & 0x1)) {
                        idxUse = j;
                        break;
                    }
                }
                if (idxUse == -1) {
                    bkPrintf("EnumDevicesCallback: No devices available\n");
                    return DIENUM_CONTINUE;
                }
                bkPrintf("EnumDevicesCallback: Matching first free device (%d)\n", idxUse);
            }
            bInputInfo.devices[idxUse].guid = lpddi->guidInstance;
            HRESULT hr = bInputInfo.pDI->CreateDevice(lpddi->guidInstance, &bInputInfo.devices[idxUse].device, NULL);
            if (FAILED(hr)) {
                if (bInputEnumAdditionalDebug) {
                    bkPrintf("EnumDevicesCallback: Could not create JOYSTICK/GAMEPAD input device\n");
                }
                return DIENUM_CONTINUE;
            }
            hr = bInputInfo.devices[idxUse].device->SetDataFormat(&c_dfDIJoystick);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set JOYSTICK/GAMEPAD data format '%s'\n", errStr);
                if (bInputInfo.devices[idxUse].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[idxUse].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[idxUse].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            hr = bInputInfo.devices[idxUse].device->SetCooperativeLevel(bMainWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not set JOYSTICK/GAMEPAD cooperative level '%s'\n", errStr);
                if (bInputInfo.devices[idxUse].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[idxUse].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[idxUse].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            DIDEVCAPS caps;
            ZeroMemory(&caps, sizeof(caps));
            caps.dwSize = sizeof(DIDEVCAPS);
            hr = bInputInfo.devices[idxUse].device->GetCapabilities(&caps);
            if (FAILED(hr)) {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not get capabilities for JOYSTICK/GAMEPAD device '%s'\n", errStr);
                if (bInputInfo.devices[idxUse].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[idxUse].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[idxUse].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
            if (caps.dwFlags & (DIDC_POLLEDDEVICE | DIDC_POLLEDDATAFORMAT)) {
                bInputInfo.devices[idxUse].flags |= 0x2;
            }
			bInputInfo.devices[idxUse].capsSize   = caps.dwSize;
            bInputInfo.devices[idxUse].numButtons = caps.dwButtons;
            bInputInfo.devices[idxUse].numAxes    = caps.dwAxes;
            bInputInfo.devices[idxUse].numPOVs    = caps.dwPOVs;
            if (bInputEnumAdditionalDebug) {
                bkPrintf("EnumDevicesCallback: Joystick has %d buttons, %d axes and %d pov's\n", caps.dwButtons, caps.dwAxes, caps.dwPOVs);
            }
            hr = bInputInfo.devices[idxUse].device->EnumObjects(EnumDeviceObjectsCallback, &bInputInfo.devices[idxUse], 0);
            if (SUCCEEDED(hr)) {
                hr = bInputInfo.devices[idxUse].device->Acquire();
                if (FAILED(hr)) {
                    LPCSTR errStr = DXGetErrorString8A(hr);
                    bkPrintf("EnumDevicesCallback: *** WARNING *** Could not acquire JOYSTICK/GAMEPAD device '%s'\n", errStr);
                }
                bInputInfo.devices[idxUse].flags |= 0x1;
                bInputInfo.joystickCount += 1;
                return DIENUM_CONTINUE;
            } else {
                LPCSTR errStr = DXGetErrorString8A(hr);
                bkPrintf("EnumDevicesCallback: Could not enumerate input objects for JOYSTICK/GAMEPAD device '%s'\n", errStr);
                if (bInputInfo.devices[idxUse].device) {
                    ULONG refCount;
                    do {
                        refCount = bInputInfo.devices[idxUse].device->Release();
                    } while (refCount != 0);
                    bInputInfo.devices[idxUse].device = NULL;
                }
                return DIENUM_CONTINUE;
            }
        }
        return DIENUM_CONTINUE;

    default:
        if (bInputEnumAdditionalDebug) {
            bkPrintf("EnumDevicesCallback: Found unknown input device '%s' '%s' (category %d)\n", lpddi->tszInstanceName, lpddi->tszProductName);
        }
        break;
    }
    return DIENUM_CONTINUE;
}

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitInputHAL
	Purpose : initialise the input module
	Parameters : bkInit flags
	Returns : 
	Info : 
*/

void bInitInputHAL(uint32 flags) 
{
    memset(&bInputInfo, 0, sizeof(bInputInfo));

    if (flags & BKINITFLAG_NONEXCLUSIVE)
    {
        bMouseExclusiveMode = 2;
        bKeyExclusiveMode   = 2;
    }

    HINSTANCE hinst = GetModuleHandleA(NULL);
    HRESULT hr = DirectInput8Create(
        hinst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8A,
        (LPVOID *)&bInputInfo.pDI,
        NULL
    );

    if (FAILED(hr))
    {
        const char *err = DXGetErrorString8A(hr);
        bkPrintf("bInitInput: Could not create DirectInput interface (%s)\n", err);
        return;
    }

    // Enumerate attached devices.
    HRESULT hrEnum = bInputInfo.pDI->EnumDevices(
        0,                    // all device types
        EnumDevicesCallback,
        NULL,              // pvRef
        DIEDFL_ATTACHEDONLY
    );

    if (FAILED(hrEnum))
    {
        const char *err = DXGetErrorString8A(hrEnum);
        bkPrintf("bInitInput: Failed to enumerate devices '%s'\n", err);

        // Release DirectInput interface completely
        if (bInputInfo.pDI)
        {
            ULONG refCount;
            do
            {
                refCount = bInputInfo.pDI->Release();
            }
            while (refCount != 0);

            bInputInfo.pDI = NULL;
        }

        return;
    }

    // Mark input system as initialized
    bInputInfo.initialized = 1;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownInputHAL
	Purpose : shutdown the input module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownInputHAL()
{
    for (int i = 0; i < BMAXPADS + 2; ++i)
    {
        TBDeviceInfo* dev = &bInputInfo.devices[i];

        if (dev->flags & 0x01)
        {
            IDirectInputDevice8A* diDev = dev->device;

            if (diDev)
            {
                diDev->Unacquire();

                diDev->SetCooperativeLevel(bMainWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

                while (diDev->Release() != 0)
                {
                    // keep releasing until refcount is 0
                }

                dev->device = NULL;
            }
        }
    }

    if (bInputInfo.pDI)
    {
        while (bInputInfo.pDI->Release() != 0)
        {
            // keep releasing until refcount is 0
        }
    }

    memset(&bInputInfo, 0, sizeof(bInputInfo));
}


/*	--------------------------------------------------------------------------------
	Function : bReadPhysicalInputDevices
	Purpose : read the physical input devices
	Parameters : 
	Returns : 
	Info : 
*/

void bReadPhysicalInputDevices()
{
#ifdef SKIP_IDEVICES_IF_INACTIVE
	if (!bActive) return; // MG: we don't need any input devices during Alt+Tab
#endif
    HRESULT       hr;
    TBDeviceInfo* dev;

    // -------------------------------------------------------------------------
    // KEYBOARD (device slot 4)
    // -------------------------------------------------------------------------
    dev = &bInputInfo.devices[4];

    if (dev->flags & 0x01)
    {
        // Optional poll before reading state
        if (dev->flags & 0x02)
        {
            hr = dev->device->Poll();
            if (FAILED(hr))
            {
                dev->device->Acquire();
                dev->device->Poll();
            }
        }

        hr = dev->device->GetDeviceState(sizeof(bInputInfo.keyboardState),
                                         bInputInfo.keyboardState);
        if (FAILED(hr))
        {
            HRESULT hrAcquire = dev->device->Acquire();
            if (FAILED(hrAcquire))
            {
                bkPrintf(
                    "bReadPhysicalInputDevices: Could not aquire KEYBOARD '%s'\n",
                    DXGetErrorString8A(hrAcquire));
            }

            hr = dev->device->GetDeviceState(sizeof(bInputInfo.keyboardState),
                                             bInputInfo.keyboardState);
            if (FAILED(hr))
            {
                bkPrintf(
                    "bReadPhysicalInputDevices: Could not read KEYBOARD state '%s'\n",
                    DXGetErrorString8A(hr));

                // Fall back to "no keys pressed"
                memset(bInputInfo.keyboardState, 0, sizeof(bInputInfo.keyboardState));
            }
        }
    }
    else
    {
        // No keyboard – clear state
        memset(bInputInfo.keyboardState, 0, sizeof(bInputInfo.keyboardState));
    }

    // -------------------------------------------------------------------------
    // MOUSE (device slot 5) -> mouseRaw + filtered copy with simple timing
    // -------------------------------------------------------------------------
    dev = &bInputInfo.devices[5];

    if (dev->flags & 0x01)
    {
        if (dev->flags & 0x02)
        {
            hr = dev->device->Poll();
            if (FAILED(hr))
            {
                dev->device->Acquire();
                dev->device->Poll();
            }
        }

        hr = dev->device->GetDeviceState(sizeof(DIMOUSESTATE),
                                         &bInputInfo.mouseRaw);
        if (FAILED(hr))
        {
            dev->device->Acquire();
            hr = dev->device->GetDeviceState(sizeof(DIMOUSESTATE),
                                             &bInputInfo.mouseRaw);
            if (FAILED(hr))
            {
                bkPrintf(
                    "bReadPhysicalInputDevices: Could not read MOUSE state '%s'\n",
                    DXGetErrorString8A(hr));

                memset(&bInputInfo.mouseRaw, 0, sizeof(DIMOUSESTATE));
            }
        }
    }
    else
    {
        memset(&bInputInfo.mouseRaw, 0, sizeof(DIMOUSESTATE));
    }

    // Mouse filter: copy raw -> filtered if > ~40ms or sth on X/Y
    {
        DWORD now = GetTickCount();

        if ((now - bLastMouseFilterTime > 0x27) ||
            (bInputInfo.mouseRaw.lX != 0) ||
            (bInputInfo.mouseRaw.lY != 0))
        {
            bInputInfo.mouseFiltered = bInputInfo.mouseRaw;
            bLastMouseFilterTime    = now;
        }
    }

    // -------------------------------------------------------------------------
    // JOYSTICKS (device slots 0..3)
    // -------------------------------------------------------------------------
    DIJOYSTATE* joyState = bInputInfo.joyState;
    int        (*joyAxis)[6] = bInputInfo.joyAxis;

    dev = &bInputInfo.devices[0];

    for (int pad = 0; pad < BMAXPADS; ++pad, ++joyState, ++joyAxis, ++dev)
    {
        // Device not present: zero DIJOYSTATE, axes = center
        if (!(dev->flags & 0x01))
        {
            memset(joyState, 0, sizeof(DIJOYSTATE));
            for (int a = 0; a < 6; ++a)
                (*joyAxis)[a] = dev->axisCenter[a];
            continue;
        }

        // Optional poll
        if (dev->flags & 0x02)
        {
            hr = dev->device->Poll();
            if (FAILED(hr))
            {
                dev->device->Acquire();
                dev->device->Poll();
            }
        }

        // Read DIJOYSTATE
        hr = dev->device->GetDeviceState(sizeof(DIJOYSTATE), joyState);
        if (FAILED(hr))
        {
            dev->device->Acquire();
            hr = dev->device->GetDeviceState(sizeof(DIJOYSTATE), joyState);
            if (FAILED(hr))
            {
                bkPrintf(
                    "bReadPhysicalInputDevices: Could not read JOYSTICK state '%s'\n",
                    DXGetErrorString8A(hr));

                memset(joyState, 0, sizeof(DIJOYSTATE));
                for (int a = 0; a < 6; ++a)
                    (*joyAxis)[a] = dev->axisCenter[a];
                continue; // LAB_0057be33
            }
        }

        // Map first 6 DIJOYSTATE axes into joyAxis[pad][0..5]
        (*joyAxis)[0] = joyState->lX;
        (*joyAxis)[1] = joyState->lY;
        (*joyAxis)[2] = joyState->lZ;
        (*joyAxis)[3] = joyState->lRx;
        (*joyAxis)[4] = joyState->lRy;
        (*joyAxis)[5] = joyState->lRz;

        // Piecewise scaling around axisCenter using axisHalfRange/axisScaled
        for (int a = 0; a < 6; ++a)
        {
            long v      = (*joyAxis)[a];
            long center = dev->axisCenter[a];
            long half   = dev->axisHalfRange[a];
            long scaled = dev->axisScaled[a];

            if (v < center)
            {
                v += scaled;
                if (v > center)
                {
                    v = center;
                }
                else if (half != 0)
                {
                    long num = (center - v) * half;
                    long den = (half - scaled);
                    v = center - (num / den);
                }
            }
            else
            {
                v -= scaled;
                if (v < center)
                {
                    v = center;
                }
                else if (half != 0)
                {
                    long num = (v - center) * half;
                    long den = (half - scaled);
                    v = center + (num / den);
                }
            }

            (*joyAxis)[a] = v;
        }
    }
}


/*	--------------------------------------------------------------------------------
	Function : bReadInput
	Purpose : return the current value of a specific input
	Parameters : input ID, value behaviour, ptr to receive value
	Returns : OK / FAIL
	Info : 
*/

int bReadInput(int inputID, int behaviour, int *valuePtr)
{
	// MG: You're entering a crappy bodge section.
	// MG: No one (even I) have no idea how it works, but it does.
	// MG: It's very difficult to make this function even approximately similar
	// MG: to what it was originally intended by the devs
	// MG: (with correct parameters from input_ids.h, etc.).
	// MG: Also keep in mind that it was almost entirely written using AI.
	// MG: I'll definitely double-check it sooner or later, but for now,
	// MG: unfortunately, it's not a priority just because it actually works.
    // ------------------------------------------------------------------
    // Common preamble
    // ------------------------------------------------------------------
    if (!valuePtr)
        return 0;

    *valuePtr = 0;

    // Global suspend: treat as "no input", but success
    if (bInputSuspended)
        return 1;

    // Special "no device" mask: always succeeds with zero value
    if ((inputID & BIINPUT_DEVMASK) == BIINPUT_DEVMASK)
        return 1;

    // Unpack inputID fields
    int deviceIndex = (inputID >> 16) & 0x0F;     // device index (0..3 pads, 4 kbd, 5 mouse)
    int gainPercent = (inputID >> 24) & 0x7F;     // 0..127
    if (gainPercent == 0)
        gainPercent = 100;                        // default 100%

    int baseInputID = inputID & 0xFF;             // low 8 bits: Babel / mouse / pad ID
    bool invertFlag = ((inputID & BIINPUT_INVERT) != 0);

    // ------------------------------------------------------------------
    // KEYBOARD PATH (Babel key IDs via DIKToBabel inverse)
    // ------------------------------------------------------------------
    // DIKToBabel was:
    //   BabelID = enumeratedKey + (BIINPUT_KEY_FIRSTID + 1);
    //
    // Where `enumeratedKey` is the index into bDirectInputKeyMap[].
    // Here we invert that mapping, obtain DIK code and read the
    // corresponding entry from bInputInfo.keyboardState[].
    // ------------------------------------------------------------------

    if (baseInputID > BIINPUT_KEY_FIRSTID && baseInputID < BIINPUT_KEY_LASTID)
    {
        int enumeratedKey = baseInputID - (BIINPUT_KEY_FIRSTID + 1);

        if (enumeratedKey < 0 || enumeratedKey >= BDIRECTINPUTKEYMAPSIZE)
        {
            // Out-of-range Babel ID: treat as "not pressed", but success
            *valuePtr = 0;
            return 1;
        }

        int dikCode = bDirectInputKeyMap[enumeratedKey];

        // Read DirectInput-style keyboard state (0x80 bit = pressed)
        unsigned char keyState = bInputInfo.keyboardState[dikCode];

        // Replicate the original bit math:
        //   pressedMask = -((keyState & 0x80) != 0) & 0x7FF8;
        // which yields either 0 or 0x7FF8 (32760).
        unsigned pressed = ((keyState & 0x80u) != 0u);
        unsigned value   = (pressed ? 0x7FF8u : 0u);

        // Apply gainPercent (0..127%), scaled by /100 exactly as in the asm.
        value = (value * (unsigned)gainPercent) / 100u;

        *valuePtr = (int)value;
        return 1;
    }

    // ------------------------------------------------------------------
    // MOUSE PATH
    // ------------------------------------------------------------------
    // baseInputID in [2..4]  : mouse buttons (B1..B3)
    // baseInputID in [5..7]  : mouse axes   (X, Y, Z)
    //
    // Behaviour:
    //   behaviour == 1 : digital threshold (0 / +/-0x7FF8)
    //   behaviour != 1 : analog scaling of lX/lY/lZ
    //
    // BIINPUT_INVERT on an axis flips its sign (useful for bindings).
    // ------------------------------------------------------------------

    // Mouse buttons: 2..4
    if (baseInputID >= 2 && baseInputID <= 4)
    {
        int btnIndex = baseInputID - 2; // 0=L, 1=R, 2=M
        unsigned char state = bInputInfo.mouseFiltered.rgbButtons[btnIndex];

        unsigned pressed = ((state & 0x80u) != 0u);
        unsigned value   = (pressed ? 0x7FF8u : 0u);

        // Apply gain
        value = (value * (unsigned)gainPercent) / 100u;

        *valuePtr = (int)value;
        return 1;
    }

    // Mouse axes: 5 = X, 6 = Y, 7 = Z (wheel)
    if (baseInputID >= 5 && baseInputID <= 7)
    {
        long raw = 0;
        if      (baseInputID == 5) raw = bInputInfo.mouseFiltered.lX;
        else if (baseInputID == 6) raw = bInputInfo.mouseFiltered.lY;
        else if (baseInputID == 7) raw = bInputInfo.mouseFiltered.lZ;

        // Apply invert flag at the "raw" level
        if (invertFlag)
            raw = -raw;

        int value = 0;

        if (behaviour == 1)
        {
            // Digital behaviour: threshold on movement
            const int threshold = (baseInputID == 7) ? 100 : 50;

            if (raw > threshold)
                value =  0x7FF8;
            else if (raw < -threshold)
                value = -0x7FF8;
            else
                value = 0;
        }
        else
        {
            // Analog behaviour: scale raw movement into [-0x7FF8..0x7FF8]
            // Divisors chosen to approximate original feel.
            int div = (baseInputID == 7) ? 400 : 150;
            if (div == 0) div = 1;

            // raw * 0x7FF8 / div
            long scaled = (raw * 0x7FF8L) / div;
            if (scaled >  0x7FF8L) scaled =  0x7FF8L;
            if (scaled < -0x7FF8L) scaled = -0x7FF8L;

            value = (int)scaled;
        }

        // Apply gainPercent
        value = (value * gainPercent) / 100;

        *valuePtr = value;
        return 1;
    }

    // ------------------------------------------------------------------
    // JOYSTICK / GAMEPAD PATH
    // ------------------------------------------------------------------
    // Ranges (from original encoding):
    //   0x0A..0x29 : joystick buttons
    //   0x2A..0x2F : joystick axes   (X,Y,Z,Rx,Ry,Rz)
    //   0x30..0x33 : POV hats (0..3)
    //   0xBD..0xC0 : D-pad directions derived from POV[0]
    // ------------------------------------------------------------------

    // --- Gamepad buttons: 0x0A..0x29 (mapped to rgbButtons[0..31]) ---
    if (baseInputID >= 0x0A && baseInputID <= 0x29)
    {
        if (bInputInfo.devices[deviceIndex].flags & 0x01)
        {
            int buttonIndex = baseInputID - 0x0A; // 0..31 within DIJOYSTATE::rgbButtons
            unsigned char state = bInputInfo.joyState[deviceIndex].rgbButtons[buttonIndex];

            unsigned pressed = ((state & 0x80u) != 0u);
            unsigned value   = pressed ? 0x7FF8u : 0u;

            value = (value * (unsigned)gainPercent) / 100u;

            *valuePtr = (int)value;
            return 1;
        }

        // Device not active: succeed with zero
        *valuePtr = 0;
        return 1;
    }

    // --- Gamepad axes: 0x2A..0x2F (X,Y,Z,Rx,Ry,Rz) ---
    if (baseInputID >= 0x2A && baseInputID <= 0x2F)
    {
        if (bInputInfo.devices[deviceIndex].flags & 0x01)
        {
            int axisIndex = baseInputID - 0x2A; // 0..5
            LONG raw = 0;

            // Map to DIJOYSTATE axes
            switch (axisIndex)
            {
                case 0: raw = bInputInfo.joyState[deviceIndex].lX;  break;
                case 1: raw = bInputInfo.joyState[deviceIndex].lY;  break;
                case 2: raw = bInputInfo.joyState[deviceIndex].lZ;  break;
                case 3: raw = bInputInfo.joyState[deviceIndex].lRx; break;
                case 4: raw = bInputInfo.joyState[deviceIndex].lRy; break;
                case 5: raw = bInputInfo.joyState[deviceIndex].lRz; break;
            }

            // Apply inversion first
            if (invertFlag)
                raw = -raw;

            LONG center    = bInputInfo.devices[deviceIndex].axisCenter[axisIndex];
            LONG halfRange = bInputInfo.devices[deviceIndex].axisHalfRange[axisIndex];
            if (halfRange == 0)
                halfRange = 1;

            LONG diff   = raw - center;
            LONG scaled = (diff * 0x7FF8L) / halfRange;

            int outValue = 0;

            if (behaviour == 1)
            {
                // Digital behaviour: deadzone + full-scale
                const int threshold = 0x1FFE; // 8190, matches original asm threshold
                if (scaled >  threshold)      outValue =  0x7FF8;
                else if (scaled < -threshold) outValue = -0x7FF8;
                else                          outValue =  0;
            }
            else
            {
                // behaviour != 1 > analog
                if (behaviour == 2)
                {
                    // Absolute value version
                    if (scaled < 0) scaled = -scaled;
                }
                outValue = (int)scaled;
            }

            // Gain scaling
            outValue = (outValue * gainPercent) / 100;

            *valuePtr = outValue;
            return 1;
        }

        *valuePtr = 0;
        return 1;
    }

    // --- POV hats as "angle" inputs: 0x30..0x33 ---
    if (baseInputID >= 0x30 && baseInputID <= 0x33)
    {
        if (bInputInfo.devices[deviceIndex].flags & 0x01)
        {
            int povIndex = baseInputID - 0x30; // 0..3
            DWORD povRaw = bInputInfo.joyState[deviceIndex].rgdwPOV[povIndex];
            int   angle  = (int)povRaw;

            // Neutral POV is reported as -1 (0xFFFFFFFF)
            if (angle == -1)
            {
                *valuePtr = 0x7FF9; // special neutral code (matches original)
                return 1;
            }

            if (behaviour == 1)
            {
                // Digital: any non-neutral direction > full-scale
                *valuePtr = 0x7FF8;
                return 1;
            }

            // Analog: map [0..36000] into [-0x7FF8..0x7FF8]
            // Original uses a fixed-point multiply/div by 36000.
            long scaled = (angle * 0x7FF8L) / 36000L;
            *valuePtr = (int)scaled;
            return 1;
        }

        *valuePtr = 0;
        return 1;
    }

    // --- D-pad directions derived from POV[0]: 0xBD..0xC0 ---
    if (baseInputID >= 0xBD && baseInputID <= 0xC0)
    {
        if (bInputInfo.devices[deviceIndex].flags & 0x01)
        {
            // Use first POV (rgdwPOV[0]) as digital D-pad source
            int angle = (int)bInputInfo.joyState[deviceIndex].rgdwPOV[0];
            bool pressed = false;

            // Note: angles are in hundredths of a degree:
            //   0      = 0°
            //   9000   = 90°
            //   18000  = 180°
            //   27000  = 270°
            // plus diagonals 4500, 13500, 22500, 31500 (0x1194, 0x34BC, 0x57E4, 0x7B0C)
            switch (baseInputID)
            {
                case 0xBD: // Left (active at left + diagonals around)
                    if (angle == 0x7B0C || angle == 0      || angle == 0x1194)
                        pressed = true;
                    break;

                case 0xBE: // Right
                    if (angle == 0x34BC || angle == 18000  || angle == 0x57E4)
                        pressed = true;
                    break;

                case 0xBF: // Down
                    if (angle == 0x57E4 || angle == 27000  || angle == 0x7B0C)
                        pressed = true;
                    break;

                case 0xC0: // Up
                    if (angle == 0x1194 || angle == 9000   || angle == 0x34BC)
                        pressed = true;
                    break;
            }

            *valuePtr = pressed ? 0x7FF8 : 0;
            return 1;
        }

        *valuePtr = 0;
        return 1;
    }

    // ------------------------------------------------------------------
    // Unknown / unsupported inputID
    // ------------------------------------------------------------------
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bUpdateRumble
	Purpose : Ensure the rumble values on the pads are set correctly
	Parameters : 
	Returns : 
	Info : called ten times a second
*/

void bUpdateRumble()
{
        bkPrintf("*** WARNING *** bUpdateRumble was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}