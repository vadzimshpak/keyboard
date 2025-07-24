#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD VirtualHIDEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP VirtualHIDEvtDriverContextCleanup;

//
// Function to initialize the device
//
NTSTATUS
VirtualHIDCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
); 