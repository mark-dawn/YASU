#ifndef COMM_H_
#define COMM_H_

//for the vusb library
usbMsgLen_t usbFunctionSetup(unsigned char*);
usbMsgLen_t usbFunctionDescriptor(struct usbRequest*);

// initialise the communication protocols
void comm_init(void);
// send data on the comm channel
void comm_data(const uint8_t*, const uint8_t*);

#endif
