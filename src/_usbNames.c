//-----------------------------------------------------------------------
// This file provides overrides for the USB device names
//-----------------------------------------------------------------------

#include "usb_names.h"

#define VENDOR_ID		        0x16C0
#define PRODUCT_ID		        0x0289  // changed from 489 2020-06-25
#define BCD_DEVICE		        0x0211

#define MANUFACTURER_NAME	    {'h','a','r','m','o','n','i','c','S','u','b','s','y','t','e','m','s'}
#define MANUFACTURER_NAME_LEN	17

#define PRODUCT_NAME		    {'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n'}
#define PRODUCT_NAME_LEN	    16

#define SERIAL_NUMBER			{'0','0','0','0','0','0','0','2','4','0'}
#define SERIAL_NUMBER_LEN		10

// teensyExpression2 only supports one midi

#define MIDI_PORT1_NAME			{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','1'}
// #define MIDI_PORT2_NAME			{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','2'}
// #define MIDI_PORT3_NAME			{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','3'}
// #define MIDI_PORT4_NAME			{'t','e','e','n','s','y','E','x','p','r','e','s','s','i','o','n','4'}
#define MIDI_PORT_NAME_LEN	    17


struct usb_string_descriptor_struct usb_string_manufacturer_name = {
        2 + MANUFACTURER_NAME_LEN * 2,
        3,
        MANUFACTURER_NAME
};
struct usb_string_descriptor_struct usb_string_product_name = {
        2 + PRODUCT_NAME_LEN * 2,
        3,
        PRODUCT_NAME
};
struct usb_string_descriptor_struct usb_string_serial_number = {
        2 + SERIAL_NUMBER_LEN * 2,
		3,
		SERIAL_NUMBER
};

struct usb_string_descriptor_struct usb_string_midi_port1 = {
	2 + MIDI_PORT_NAME_LEN *2,
	3,
	MIDI_PORT1_NAME
};

// struct usb_string_descriptor_struct usb_string_midi_port2 = {
// 	2 + MIDI_PORT_NAME_LEN *2,
// 	3,
// 	MIDI_PORT2_NAME
// };
// struct usb_string_descriptor_struct usb_string_midi_port3 = {
// 	2 + MIDI_PORT_NAME_LEN *2,
// 	3,
// 	MIDI_PORT3_NAME
// };
// struct usb_string_descriptor_struct usb_string_midi_port4 = {
// 	2 + MIDI_PORT_NAME_LEN *2,
// 	3,
// 	MIDI_PORT4_NAME
// };

// end of _usbNames.c
