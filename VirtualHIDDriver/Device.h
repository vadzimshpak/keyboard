#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>

//
// Custom IOCTL codes for communication with user mode
//
#define IOCTL_KEYBOARD_INPUT CTL_CODE(FILE_DEVICE_KEYBOARD, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUSE_INPUT    CTL_CODE(FILE_DEVICE_MOUSE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Device context structure
//
typedef struct _DEVICE_CONTEXT
{
    VHFHANDLE VhfKeyboardHandle;
    VHFHANDLE VhfMouseHandle;
    WDFQUEUE  DefaultQueue;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// Function to get device context
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// HID Report structures
//
#pragma pack(push, 1)

typedef struct _KEYBOARD_INPUT_REPORT
{
    BYTE Modifiers;    // Modifier keys bitmap
    BYTE Reserved;     // Reserved byte
    BYTE KeyCodes[6];  // Up to 6 simultaneous key codes

} KEYBOARD_INPUT_REPORT, *PKEYBOARD_INPUT_REPORT;

typedef struct _MOUSE_INPUT_REPORT
{
    BYTE Buttons;      // Mouse button bitmap
    BYTE X;            // X movement (relative)
    BYTE Y;            // Y movement (relative)
    BYTE Wheel;        // Wheel movement

} MOUSE_INPUT_REPORT, *PMOUSE_INPUT_REPORT;

#pragma pack(pop)

//
// User mode input structures
//
typedef struct _KEYBOARD_INPUT_DATA
{
    BYTE VirtualKey;
    BOOL KeyDown;

} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;

typedef struct _MOUSE_INPUT_DATA
{
    LONG DeltaX;
    LONG DeltaY;
    BYTE ButtonFlags;
    SHORT WheelDelta;

} MOUSE_INPUT_DATA, *PMOUSE_INPUT_DATA;

//
// Function declarations
//
NTSTATUS
VirtualHIDCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

NTSTATUS
VirtualHIDInitializeVhf(
    _In_ WDFDEVICE Device
);

VOID
VirtualHIDCleanupVhf(
    _In_ PDEVICE_CONTEXT DeviceContext
);

BYTE
VirtualKeyToHidScanCode(
    _In_ BYTE VirtualKey
);

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL VirtualHIDEvtIoDeviceControl;
EVT_WDF_OBJECT_CONTEXT_CLEANUP VirtualHIDEvtDeviceContextCleanup;

EVT_VHF_READY_FOR_NEXT_READ_REPORT VirtualHIDEvtVhfReadyForNextReadReport;
EVT_VHF_ASYNC_OPERATION VirtualHIDEvtVhfAsyncOperationGetFeature;
EVT_VHF_ASYNC_OPERATION VirtualHIDEvtVhfAsyncOperationSetFeature; 