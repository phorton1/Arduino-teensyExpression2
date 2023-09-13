//-----------------------------------------------------------------------
// This file provides overrides for the USB device names
//-----------------------------------------------------------------------

#include "usb_names.h"
#include <Arduino.h>	// for memcpy()

// as long as I allow enough room in the declaration for any overwrites,
// I can overwrite these at my will.



struct usb_string_descriptor_struct usb_string_manufacturer_name = {
	2 + 17 * 2,
	3,
	{'h','a','r','m','o','n','i','c','S','u','b','s','y','t','e','m','s'}
};
struct usb_string_descriptor_struct usb_string_product_name = {
	2 + 18 * 2,
	3,
	{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','v','2',	0,0, 0,0,0,0,0,0,0,0,0,0}		// allow 30
};
struct usb_string_descriptor_struct usb_string_serial_number = {
	2 + 4 * 2,
	3,
	{'T','E','0','1', 0,0,0,0,0,0,0,0,0,0 }			// allow 14
};
struct usb_string_descriptor_struct usb_string_midi_port1 = {
	2 + 17 * 2,
	3,
	{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','1'}		 // stays the same

};
struct usb_string_descriptor_struct usb_string_midi_port2 = {
	2 + 17 * 2,
	3,
	{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','2',	0,0,0, 0,0,0,0,0,0,0,0,0,0}		// allow 30
};
struct usb_string_descriptor_struct usb_string_midi_port3 = {
	2 + 17 * 2,
	3,
	{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','3',	0,0,0, 0,0,0,0,0,0,0,0,0,0}		// allow 30
};
struct usb_string_descriptor_struct usb_string_midi_port4 = {
	2 + 17 * 2,
	3,
	{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','4',	0,0,0, 0,0,0,0,0,0,0,0,0,0}		// allow 30
};



static uint16_t fishman_name[] =		{'F','i','s','h','m','a','n',' ','T','r','i','p','l','e','P','l','a','y' ,0};
static uint16_t fishman_serial[] =		{'F','T','P','1', 0};
static uint16_t fishman_midi_in[] =		{'M','I','D','I','I','N','2',' ','(','F','i','s','h','m','a','n',' ','T','r','i','p','l','e','P','l','a','y',')' ,0};
static uint16_t fishman_midi_out[] =	{'M','I','D','I','O','U','T','2',' ','(','F','i','s','h','m','a','n',' ','T','r','i','p','l','e','P','l','a','y',')' ,0};


static void setDescriptor(struct usb_string_descriptor_struct *desc, uint16_t *in, int max)
{
	int len = 0;
	uint16_t *out = &desc->wString[0];
	while (len < max && *in)
	{
		*out++ = *in++;
		len++;
	}
	desc->bLength = 2 + len * 2;
}


void setUSBSerialNum()
	// copies the actually teensy serial number to after the TE or FTP
	// in the defined structure
{
	extern struct usb_string_descriptor_struct usb_string_serial_number_default;
	int len = usb_string_serial_number.wString[0] == 'F' ? 3 : 2;
	memcpy(&usb_string_serial_number.wString[len],usb_string_serial_number_default.wString,10 * sizeof(uint16_t));
	usb_string_serial_number.bLength = 2 + (len + 10) * 2;
}



void setFTPDescriptors()
{
	setDescriptor(&usb_string_product_name,		fishman_name,		30);
	setDescriptor(&usb_string_serial_number,	fishman_serial, 	10);
	setDescriptor(&usb_string_midi_port2,		fishman_name,		30);
	setDescriptor(&usb_string_midi_port3,		fishman_midi_in,	30);
	setDescriptor(&usb_string_midi_port4,		fishman_midi_out,	30);
}


// end of _usbNames.c
