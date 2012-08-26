/* Copyright (c) 2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Platform.h"
#include "HIDJoystick.h"
//#include "USBDesc.h"

#if defined(USBCON)
#ifdef HID_ENABLED

// *** Add a joystick too

Joystick_ Joystick;

//================================================================================
//================================================================================

//	HID report descriptor

#define RAWHID_USAGE_PAGE	0xFFC0
#define RAWHID_USAGE		0x0C00
#define RAWHID_TX_SIZE 64
#define RAWHID_RX_SIZE 64

extern const u8 _hidReportDescriptor[] PROGMEM;
const u8 _hidReportDescriptor[] = {
	
// *** Here is where the RAW_HID has been converted to a Joystick device
// *** Adapted from helmpcb.com/electronics/usb-joystick
// *** I haven't implemented the hat-switch yet
	//	JOYSTICK HID
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x04,                    // USAGE (Joystick)
	0xA1, 0x01,				       // Collection (Application)
    0x85, 0x03,                    // REPORT_ID (3)

	0x05, 0x02,                    // USAGE_PAGE (Simulation controls)
	0x09, 0xBB,                    // USAGE (Throttle)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x26, 0xFF, 0x00,              // LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    // REPORT_SIZE (8)
	0x95, 0x01,                    // REPORT_COUNT (1)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)

	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    // USAGE (Pointer)
	0xA1, 0x00,                    // Collection (Physical)
	0x09, 0x32,                    // USAGE (z)
	0x09, 0x30,                    // USAGE (x)
	0x09, 0x31,                    // USAGE (y)
	0x95, 0x03,                    // REPORT_COUNT (3)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)
    0xC0,                          // END_COLLECTION
    
    0x05, 0x09,                    // USAGE_PAGE (Button)
    0x19, 0x01,                    // USAGE_MINIMUM (button 1)
    0x29, 0x08,                    // USAGE_MAXIMUM (button 8)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x25, 0x01,                    // LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    // REPORT_SIZE (1)
    0x95, 0x08,                    // REPORT_COUNT (8)
    0x55, 0x00,                    // UNIT_EXPONENT (0)
    0x65, 0x00,                    // UNIT (None)
    0x81, 0x02,                    // Input (Data,Var,Abs)
	0xC0					// end collection
};

extern const HIDDescriptor _hidInterface PROGMEM;
const HIDDescriptor _hidInterface =
{
	D_INTERFACE(HID_INTERFACE,1,3,0,0),
	D_HIDREPORT(sizeof(_hidReportDescriptor)),
	D_ENDPOINT(USB_ENDPOINT_IN (HID_ENDPOINT_INT),USB_ENDPOINT_TYPE_INTERRUPT,0x40,0x01)
};

//================================================================================
//================================================================================
//	Driver

u8 _hid_protocol = 1;
u8 _hid_idle = 1;

#define WEAK __attribute__ ((weak))
#define WEAK 

int WEAK HID_GetInterface(u8* interfaceNum)
{
	interfaceNum[0] += 1;	// uses 1
	return USB_SendControl(TRANSFER_PGM,&_hidInterface,sizeof(_hidInterface));
}

int WEAK HID_GetDescriptor(int i)
{
	return USB_SendControl(TRANSFER_PGM,_hidReportDescriptor,sizeof(_hidReportDescriptor));
}

void WEAK HID_SendReport(u8 id, const void* data, int len)
{
	USB_Send(HID_TX, &id, 1);
	USB_Send(HID_TX | TRANSFER_RELEASE,data,len);
}

bool WEAK HID_Setup(Setup& setup)
{
	u8 r = setup.bRequest;
	u8 requestType = setup.bmRequestType;
	if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType)
	{
		if (HID_GET_REPORT == r)
		{
			//HID_GetReport();
			return true;
		}
		if (HID_GET_PROTOCOL == r)
		{
			//Send8(_hid_protocol);	// TODO
			return true;
		}
	}
	
	if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType)
	{
		if (HID_SET_PROTOCOL == r)
		{
			_hid_protocol = setup.wValueL;
			return true;
		}

		if (HID_SET_IDLE == r)
		{
			_hid_idle = setup.wValueL;
			return true;
		}
	}
	return false;
}

//================================================================================
//================================================================================
//	Joystick
//  Usage: Joystick.move(throttle, brake, x, y, buttons)
//  throttle & brake max = 0, min = 255
//  x & y forward/left = 0, centre = 127, back/right = 255
//  8 buttons packed into 1 byte

Joystick_::Joystick_()
{
}

void Joystick_::begin(void) 
{
}

void Joystick_::end(void) 
{
}

void Joystick_::move(uint8_t throttle, uint8_t brake, uint8_t x, uint8_t y, uint8_t buttons)
{
    u8 j[5];
    j[0] = throttle;
    j[1] = brake;
    j[2] = x;
    j[3] = y;
    j[4] = buttons;
    //HID_SendReport(Report number, array of values in same order as HID descriptor, length)
    HID_SendReport(3, j, 5);
}

#endif

#endif /* if defined(USBCON) */
