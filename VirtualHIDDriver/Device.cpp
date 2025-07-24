#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>
#include "Device.h"
#include "Trace.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, VirtualHIDCreateDevice)
#pragma alloc_text (PAGE, VirtualHIDInitializeVhf)
#endif

//
// Device Interface GUID for the virtual HID device
// {12345678-1234-1234-1234-123456789012}
//
DEFINE_GUID(GUID_DEVINTERFACE_VIRTUALHID,
    0x12345678, 0x1234, 0x1234, 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12);

//
// Standard HID Report Descriptor for Keyboard
//
const UCHAR g_KeyboardReportDescriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (101)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x29, 0x65,        //   Usage Maximum (0x65)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

//
// Standard HID Report Descriptor for Mouse
//
const UCHAR g_MouseReportDescriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x03,        //     Usage Maximum (0x03)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x05,        //     Report Size (5)
    0x81, 0x01,        //     Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

//
// Virtual Key to HID Scan Code mapping table
//
BYTE VirtualKeyToHidScanCode(BYTE virtualKey)
{
    switch (virtualKey)
    {
        // Letters A-Z
        case 'A': case 'a': return 0x04;
        case 'B': case 'b': return 0x05;
        case 'C': case 'c': return 0x06;
        case 'D': case 'd': return 0x07;
        case 'E': case 'e': return 0x08;
        case 'F': case 'f': return 0x09;
        case 'G': case 'g': return 0x0A;
        case 'H': case 'h': return 0x0B;
        case 'I': case 'i': return 0x0C;
        case 'J': case 'j': return 0x0D;
        case 'K': case 'k': return 0x0E;
        case 'L': case 'l': return 0x0F;
        case 'M': case 'm': return 0x10;
        case 'N': case 'n': return 0x11;
        case 'O': case 'o': return 0x12;
        case 'P': case 'p': return 0x13;
        case 'Q': case 'q': return 0x14;
        case 'R': case 'r': return 0x15;
        case 'S': case 's': return 0x16;
        case 'T': case 't': return 0x17;
        case 'U': case 'u': return 0x18;
        case 'V': case 'v': return 0x19;
        case 'W': case 'w': return 0x1A;
        case 'X': case 'x': return 0x1B;
        case 'Y': case 'y': return 0x1C;
        case 'Z': case 'z': return 0x1D;
        
        // Numbers 1-9, 0
        case '1': return 0x1E;
        case '2': return 0x1F;
        case '3': return 0x20;
        case '4': return 0x21;
        case '5': return 0x22;
        case '6': return 0x23;
        case '7': return 0x24;
        case '8': return 0x25;
        case '9': return 0x26;
        case '0': return 0x27;
        
        // Common keys
        case VK_RETURN: return 0x28;    // Enter
        case VK_ESCAPE: return 0x29;    // Escape
        case VK_BACK: return 0x2A;      // Backspace
        case VK_TAB: return 0x2B;       // Tab
        case VK_SPACE: return 0x2C;     // Space
        
        default: return 0x00; // No mapping
    }
}

NTSTATUS
VirtualHIDCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. Don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
    UNICODE_STRING symbolicLinkName;

    PAGED_CODE();

    //
    // Configure device attributes to specify the size of device context
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    deviceAttributes.EvtCleanupCallback = VirtualHIDEvtDeviceContextCleanup;

    //
    // Create a framework device object.
    //
    status = WdfDeviceCreate(&DeviceInit,
                           &deviceAttributes,
                           &device);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreate failed %!STATUS!", status);
        return status;
    }

    //
    // Get the device context and initialize it
    //
    deviceContext = DeviceGetContext(device);
    RtlZeroMemory(deviceContext, sizeof(DEVICE_CONTEXT));

    //
    // Create device interface so user-mode applications can find and talk to us
    //
    status = WdfDeviceCreateDeviceInterface(device,
                                           &GUID_DEVINTERFACE_VIRTUALHID,
                                           NULL);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreateDeviceInterface failed %!STATUS!", status);
        return status;
    }

    //
    // Create a symbolic link for the device
    //
    RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\VirtualHIDDevice");
    status = WdfDeviceCreateSymbolicLink(device, &symbolicLinkName);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreateSymbolicLink failed %!STATUS!", status);
        // Don't fail device creation because of symbolic link failure
        // The device interface should still work
    }

    //
    // Initialize the I/O Package and any Queues
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
    queueConfig.EvtIoDeviceControl = VirtualHIDEvtIoDeviceControl;

    status = WdfIoQueueCreate(device,
                             &queueConfig,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &deviceContext->DefaultQueue);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    //
    // Initialize VHF for both keyboard and mouse
    //
    status = VirtualHIDInitializeVhf(device);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VirtualHIDInitializeVhf failed %!STATUS!", status);
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "Device creation completed successfully");

    return status;
}

NTSTATUS
VirtualHIDInitializeVhf(
    _In_ WDFDEVICE Device
)
/*++

Routine Description:

    Initializes the VHF (Virtual HID Framework) for both keyboard and mouse devices.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_CONTEXT deviceContext;
    VHF_CONFIG vhfConfig;
    NTSTATUS status;

    PAGED_CODE();

    deviceContext = DeviceGetContext(Device);

    //
    // Initialize VHF for Keyboard
    //
    VHF_CONFIG_INIT(&vhfConfig, 
                    WdfDeviceWdmGetDeviceObject(Device),
                    sizeof(g_KeyboardReportDescriptor),
                    g_KeyboardReportDescriptor);

    vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    // Set device attributes for keyboard
    vhfConfig.VendorID = 0x045E;  // Microsoft vendor ID
    vhfConfig.ProductID = 0x0750; // Keyboard product ID
    vhfConfig.VersionNumber = 0x0001;

    status = VhfCreate(&vhfConfig, &deviceContext->VhfKeyboardHandle);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfCreate for keyboard failed %!STATUS!", status);
        return status;
    }

    status = VhfStart(deviceContext->VhfKeyboardHandle);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfStart for keyboard failed %!STATUS!", status);
        VhfDelete(deviceContext->VhfKeyboardHandle, TRUE);
        deviceContext->VhfKeyboardHandle = NULL;
        return status;
    }

    //
    // Initialize VHF for Mouse
    //
    VHF_CONFIG_INIT(&vhfConfig,
                    WdfDeviceWdmGetDeviceObject(Device),
                    sizeof(g_MouseReportDescriptor),
                    g_MouseReportDescriptor);

    vhfConfig.EvtVhfReadyForNextReadReport = VirtualHIDEvtVhfReadyForNextReadReport;
    vhfConfig.EvtVhfAsyncOperationGetFeature = VirtualHIDEvtVhfAsyncOperationGetFeature;
    vhfConfig.EvtVhfAsyncOperationSetFeature = VirtualHIDEvtVhfAsyncOperationSetFeature;

    // Set device attributes for mouse with different product ID
    vhfConfig.VendorID = 0x045E;  // Microsoft vendor ID
    vhfConfig.ProductID = 0x0040; // Mouse product ID (different from keyboard)
    vhfConfig.VersionNumber = 0x0001;

    status = VhfCreate(&vhfConfig, &deviceContext->VhfMouseHandle);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfCreate for mouse failed %!STATUS!", status);
        // Clean up keyboard on mouse failure
        VhfDelete(deviceContext->VhfKeyboardHandle, TRUE);
        deviceContext->VhfKeyboardHandle = NULL;
        return status;
    }

    status = VhfStart(deviceContext->VhfMouseHandle);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "VhfStart for mouse failed %!STATUS!", status);
        // Clean up both devices on mouse start failure
        VhfDelete(deviceContext->VhfKeyboardHandle, TRUE);
        VhfDelete(deviceContext->VhfMouseHandle, TRUE);
        deviceContext->VhfKeyboardHandle = NULL;
        deviceContext->VhfMouseHandle = NULL;
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "VHF initialization completed successfully");

    return STATUS_SUCCESS;
}

VOID
VirtualHIDEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
)
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue - Handle to the framework queue object that is associated with the
            I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code for the request

Return Value:

    VOID

--*/
{
    WDFDEVICE device;
    PDEVICE_CONTEXT deviceContext;
    NTSTATUS status = STATUS_SUCCESS;
    PVOID inputBuffer = NULL;
    size_t inputBufferSize = 0;

    UNREFERENCED_PARAMETER(OutputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    deviceContext = DeviceGetContext(device);

    //
    // Validate device context
    //
    if (deviceContext->VhfKeyboardHandle == NULL || deviceContext->VhfMouseHandle == NULL) {
        status = STATUS_DEVICE_NOT_READY;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "VHF devices not initialized");
        goto Exit;
    }

    //
    // Get input buffer
    //
    if (InputBufferLength > 0) {
        status = WdfRequestRetrieveInputBuffer(Request,
                                              InputBufferLength,
                                              &inputBuffer,
                                              &inputBufferSize);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveInputBuffer failed %!STATUS!", status);
            goto Exit;
        }
    }

    switch (IoControlCode) {

    case IOCTL_KEYBOARD_INPUT:
        {
            if (InputBufferLength < sizeof(KEYBOARD_INPUT_DATA)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PKEYBOARD_INPUT_DATA keyboardData = (PKEYBOARD_INPUT_DATA)inputBuffer;
            KEYBOARD_INPUT_REPORT keyboardReport = { 0 };
            HID_XFER_PACKET packet;

            // Improved key mapping using lookup table
            if (keyboardData->KeyDown) {
                BYTE scanCode = VirtualKeyToHidScanCode(keyboardData->VirtualKey);
                if (scanCode != 0x00) {
                    keyboardReport.KeyCodes[0] = scanCode;
                }
            }
            // For key release, send empty report (all zeros)

            packet.reportBuffer = (PUCHAR)&keyboardReport;
            packet.reportBufferLen = sizeof(keyboardReport);
            packet.reportId = 0;

            status = VhfReadReportSubmit(deviceContext->VhfKeyboardHandle, &packet);

            TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, 
                       "Keyboard input: VKey=0x%02X, KeyDown=%d, Status=%!STATUS!", 
                       keyboardData->VirtualKey, keyboardData->KeyDown, status);
        }
        break;

    case IOCTL_MOUSE_INPUT:
        {
            if (InputBufferLength < sizeof(MOUSE_INPUT_DATA)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PMOUSE_INPUT_DATA mouseData = (PMOUSE_INPUT_DATA)inputBuffer;
            MOUSE_INPUT_REPORT mouseReport = { 0 };
            HID_XFER_PACKET packet;

            mouseReport.Buttons = mouseData->ButtonFlags;
            // Clamp movement to valid range (-127 to 127)
            mouseReport.X = (BYTE)max(-127, min(127, mouseData->DeltaX));
            mouseReport.Y = (BYTE)max(-127, min(127, mouseData->DeltaY));
            mouseReport.Wheel = (BYTE)max(-127, min(127, mouseData->WheelDelta));

            packet.reportBuffer = (PUCHAR)&mouseReport;
            packet.reportBufferLen = sizeof(mouseReport);
            packet.reportId = 0;

            status = VhfReadReportSubmit(deviceContext->VhfMouseHandle, &packet);

            TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, 
                       "Mouse input: DX=%d, DY=%d, Buttons=0x%02X, Status=%!STATUS!", 
                       mouseData->DeltaX, mouseData->DeltaY, mouseData->ButtonFlags, status);
        }
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Unknown IOCTL: 0x%08X", IoControlCode);
        break;
    }

Exit:
    WdfRequestComplete(Request, status);
}

VOID
VirtualHIDEvtVhfReadyForNextReadReport(
    _In_ VHFHANDLE VhfHandle
)
/*++

Routine Description:

    This callback function is invoked when VHF is ready to accept the next input report.

Arguments:

    VhfHandle - Handle to the VHF instance.

Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(VhfHandle);
    
    // This callback indicates that VHF is ready for the next input report
    // In a real implementation, you might use this to manage a queue of pending inputs
    TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_DEVICE, "VHF ready for next read report");
}

VOID
VirtualHIDEvtVhfAsyncOperationGetFeature(
    _In_     VHFHANDLE VhfHandle,
    _In_     VHFOPERATIONHANDLE VhfOperationHandle,
    _In_opt_ PVOID VhfOperationContext,
    _In_     PHID_XFER_PACKET HidTransferPacket
)
/*++

Routine Description:

    This callback function is invoked when VHF receives a Get Feature request.

Arguments:

    VhfHandle - Handle to the VHF instance.
    VhfOperationHandle - Handle to the operation.
    VhfOperationContext - Context for the operation.
    HidTransferPacket - Pointer to the HID transfer packet.

Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(VhfHandle);
    UNREFERENCED_PARAMETER(VhfOperationContext);
    UNREFERENCED_PARAMETER(HidTransferPacket);

    // Complete the operation with no data
    VhfAsyncOperationComplete(VhfOperationHandle, STATUS_SUCCESS);
}

VOID
VirtualHIDEvtVhfAsyncOperationSetFeature(
    _In_     VHFHANDLE VhfHandle,
    _In_     VHFOPERATIONHANDLE VhfOperationHandle,
    _In_opt_ PVOID VhfOperationContext,
    _In_     PHID_XFER_PACKET HidTransferPacket
)
/*++

Routine Description:

    This callback function is invoked when VHF receives a Set Feature request.

Arguments:

    VhfHandle - Handle to the VHF instance.
    VhfOperationHandle - Handle to the operation.
    VhfOperationContext - Context for the operation.
    HidTransferPacket - Pointer to the HID transfer packet.

Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(VhfHandle);
    UNREFERENCED_PARAMETER(VhfOperationContext);
    UNREFERENCED_PARAMETER(HidTransferPacket);

    // Complete the operation successfully
    VhfAsyncOperationComplete(VhfOperationHandle, STATUS_SUCCESS);
} 

VOID
VirtualHIDCleanupVhf(
    _In_ PDEVICE_CONTEXT DeviceContext
)
/*++

Routine Description:

    Cleans up VHF resources for both keyboard and mouse devices.

Arguments:

    DeviceContext - Pointer to the device context structure.

Return Value:

    VOID

--*/
{
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "Cleaning up VHF resources");

    if (DeviceContext->VhfKeyboardHandle != NULL)
    {
        VhfDelete(DeviceContext->VhfKeyboardHandle, TRUE);
        DeviceContext->VhfKeyboardHandle = NULL;
    }

    if (DeviceContext->VhfMouseHandle != NULL)
    {
        VhfDelete(DeviceContext->VhfMouseHandle, TRUE);
        DeviceContext->VhfMouseHandle = NULL;
    }
}

VOID
VirtualHIDEvtDeviceContextCleanup(
    _In_ WDFOBJECT Object
)
/*++

Routine Description:

    Called when the device context is being cleaned up.

Arguments:

    Object - Handle to the framework object.

Return Value:

    VOID

--*/
{
    PDEVICE_CONTEXT deviceContext;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "Device context cleanup");

    deviceContext = DeviceGetContext((WDFDEVICE)Object);
    if (deviceContext != NULL)
    {
        VirtualHIDCleanupVhf(deviceContext);
    }
} 