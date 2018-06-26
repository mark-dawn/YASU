#include <inttypes.h>
#include "usbdrv.h"
#include "comm.h"
#include <avr/eeprom.h>
#include <util/delay.h>

// USB config descriptor
PROGMEM const char usbDescriptorConfiguration[] = {
    9,                      /* sizeof(usbDescriptorConfiguration): length of descriptor in bytes */
    USBDESCR_CONFIG,        /* descriptor type */
    9 + (2*(9+9+7)), 0,     /* total length of data returned (including inlined descriptors) */
    2,                      /* number of interfaces in this configuration */
    1,                      /* index of this configuration */
    0,                      /* configuration name string index */
    (1<<7),                 /* attributes bitmap, 1,SELFPWR,WAKEUP,0,0,0,0,0 */
    USB_CFG_MAX_BUS_POWER/2,/* max USB current in 2mA units  */

    // interface descriptor follows inline
    9,                      /* sizeof(usbDescrInterface): length of descriptor in bytes */
    USBDESCR_INTERFACE,     /* descriptor type */
    0,                      /* index of this interface */
    0,                      /* alternate setting for this interface */
    1,                      /* endpoints excl 0: number of endpoint descriptors to follow */
    USB_CFG_INTERFACE_CLASS,/* those are defines in usbconfig.h */
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    3,                      /* string index for interface */

    // HID descriptor follows inline
    9,                      /* sizeof(usbDescrHID): length of descriptor in bytes */
    USBDESCR_HID,           /* descriptor type: HID */
    0x01, 0x01,             /* BCD representation of HID version */
    0x00,                   /* target country code */
    0x01,                   /* number of HID Report (or other HID class) Descriptor infos to follow */
    0x22,                   /* descriptor type: report */
    USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH, 0,  /* total length of report descriptor */

    // endpoint descriptor follows inline
    7,                      /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,      /* descriptor type = endpoint */
    (char)0x81,             /* basic info bitmap, I/O,0,0,0,0,0,2:[END#] */
    0x03,                   /* attrib bitmap: 0,0,2:[USAGE],2:[SYNCHRO],2:[TYPE]*/
    8, 0,                   /* maximum packet size */
    USB_CFG_INTR_POLL_INTERVAL, /* in ms */

    // interface #2 descriptor follows inline
    9,                      /* sizeof(usbDescrInterface): length of descriptor in bytes */
    USBDESCR_INTERFACE,     /* descriptor type */
    1,                      /* index of this interface */
    0,                      /* alternate setting for this interface */
    1,                      /* endpoints excl 0: number of endpoint descriptors to follow */
    USB_CFG_INTERFACE_CLASS,/* those are defines in usbconfig.h */
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    4,                      /* string index for interface */

    // HID descriptor follows inline
    9,                      /* sizeof(usbDescrHID): length of descriptor in bytes */
    USBDESCR_HID,           /* descriptor type: HID */
    0x01, 0x01,             /* BCD representation of HID version */
    0x00,                   /* target country code */
    0x01,                   /* number of HID Report (or other HID class) Descriptor infos to follow */
    0x22,                   /* descriptor type: report */
    USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH, 0,  /* total length of report descriptor */

    // endpoint descriptor follows inline
    7,                      /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,      /* descriptor type = endpoint */
    (char)(0x80|USB_CFG_EP3_NUMBER),/* basic info bitmap, I/O,0,0,0,0,0,2:[END#] */
    0x03,                   /* attrib bitmap: 0,0,2:[USAGE],2:[SYNCHRO],2:[TYPE]*/
    8, 0,                   /* maximum packet size */
    USB_CFG_INTR_POLL_INTERVAL, /* in ms */
};

// HID report Descriptor
// 3 bytes packaging
// 0: Report ID
// 1: [Z X Y R C A B S]
// 2: [x x y y 0 0 0 L]
PROGMEM const char usbHidReportDescriptor[] = {
    0x05, 0x01,     //  USAGE_PAGE (Generic Desktop)
    0x09, 0x05,     //  USAGE (Gamepad)
    0xa1, 0x01,     //  COLLECTION (Application)
//    0x09, 0x01,     //      USAGE (Pointer)
    0xa1, 0x00,     //      COLLECTION (Physical)
    0x05, 0x09,     //          USAGE_PAGE (Button)
    0x19, 0x01,     //          USAGE_MIN (Btn1)
    0x29, 0x09,     //          USAGE_MAX (Btn9)
    0x15, 0x00,     //          LOGICAL_MIN (0)
    0x25, 0x01,     //          LOGICAL_MAX (1)
    0x75, 0x01,     //          REPORT_SIZE (1)
    0x95, 0x09,     //          REPORT_COUNT(9)
    0x81, 0x02,     //          INPUT (Data, Var, Abs)
    0x75, 0x03,     //          REPORT_SIZE (3)
    0x95, 0x01,     //          REPORT_COUNT(1)
    0x81, 0x03,     //          INPUT (Cnst, Var, Abs)
    0x05, 0x01,     //          USAGE_PAGE (Generic_Desktop)
    0x09, 0x30,     //          USAGE (X)
    0x09, 0x31,     //          USAGE (Y)
    0x15, 0xff,     //          LOGICAL_MIN (-1)
    0x25, 0x01,     //          LOGICAL_MAX (1)
    0x75, 0x02,     //          REPORT_SIZE (2)
    0x95, 0x02,     //          REPORT_COUNT(2)
    0x81, 0x02,     //          INPUT (Data, Var, Abs)
    0xc0,           //      END_COLLECTION
    0xc0            //  END_COLLECTION
};

static const char LangDescriptor[] = {
    0x04,       // length
    0x03,       // code for string descriptor
    0x09, 0x04  // code for english(US)
};
static PROGMEM const char VendorStr[]   = "Lainanon";
static PROGMEM const char SatStr[] = "USB-Saturn #n";
static volatile uint8_t idleRate; // repeat rate for keyboards, should be unused
static uint8_t dummy_report[] = {0x00, 0x00}; //dummy report for endpoint0

// for the vusb library
// this are the answers to control messages on endpoint 0
usbMsgLen_t usbFunctionSetup(unsigned char data[8]) {
    usbRequest_t *rq = (void *)data;
    /* The following requests are never used(?),
     * as we are using endpoint 1 and 3 for interrupts.
     * But since they are required by the specification,
     * we implement them.
     */
    if(((*rq).bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
        if((*rq).bRequest == USBRQ_HID_GET_REPORT) {
            // wValue: ReportType (highbyte), ReportID (lowbyte)
            usbMsgPtr = (void *)dummy_report;
            return sizeof(dummy_report);
        }
        else if((*rq).bRequest == USBRQ_HID_GET_IDLE) {
            usbMsgPtr = &idleRate;
            return 1;
        }
        else if((*rq).bRequest == USBRQ_HID_SET_IDLE) {
            idleRate = (*rq).wValue.bytes[1];
        }
    }
    return 0; // by default don't return any data
}

// to send different strings on different endpoints
usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq) {
    // get a buffer that's the biggest between the strings we have,
    // times two for wchar encoding and two extra bytes
    static char buf[(sizeof(SatStr) > sizeof(VendorStr) ?
            sizeof(SatStr) : sizeof(VendorStr)) * 2 + 2];
    uchar interface = 0;
    uint8_t len;
    const char* rom_ptr;
    uint8_t str_req = (*rq).wValue.bytes[0];
    if (str_req == 0) { // language req
        usbMsgPtr = (usbMsgPtr_t)LangDescriptor;
        return 4;
    }
    else if (str_req == 1) { // Vendor
        rom_ptr = VendorStr;
        len = sizeof(VendorStr);
    }
    else if (str_req == 2) { // Product
        rom_ptr = SatStr;
        len = sizeof(SatStr) - 3 * sizeof(char); // get rid of the pad number
    }
    else if (str_req == 3 || str_req == 4) { // interface 1 or 3
        rom_ptr = SatStr;
        len = sizeof(SatStr);
        interface = '1' + (str_req - 3);
    }
    else { return 0; }
    // copy from flash ROM to RAM, while converting to wchar
    char* cur = buf;
    len *= 2;
    do {
        cur += 2; // leave two bytes for length and type, then leave wchar stride
        *cur = pgm_read_byte(rom_ptr++);
    } while (*cur);
    buf[0] = len;
    buf[1] = 0x03; // code for str descriptor
    if (interface) {
        // patch in the last spot the interface number
        cur -= 2;
        *cur = interface;
    }
    usbMsgPtr = (usbMsgPtr_t)buf;
    return len;
}

// for the main loop
void comm_init(void) {
    usbInit();
    // re-enumerate device
    usbDeviceDisconnect();
    _delay_ms(250);
    usbDeviceConnect();
    return;
}

void comm_data(const uint8_t* buf_1, const uint8_t* buf_3) {
    usbSetInterrupt(buf_1, 2);
    usbSetInterrupt3(buf_3, 2);
    usbPoll();
    return;
}
