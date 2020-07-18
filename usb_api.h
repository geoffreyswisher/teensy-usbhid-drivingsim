#ifndef USBserial_h_
#define USBserial_h_

#include <inttypes.h>

#include "keylayouts.h"
#include "Print.h"
#include "Stream.h"

extern uint8_t joystick_report_data[8];

class usb_joystick_class
{
	public:
	usb_joystick_class() { manual_mode = 0; }
	inline void button(uint8_t button, bool val) {
		button--;
		uint8_t mask = (1 << (button & 7));
		if (val) {
			if (button < 8) joystick_report_data[0] |= mask;
			else if (button < 16) joystick_report_data[1] |= mask;
			else if (button < 24) joystick_report_data[2] |= mask;
			else if (button < 32) joystick_report_data[3] |= mask;
		} else {
			mask = ~mask;
			if (button < 8) joystick_report_data[0] &= mask;
			else if (button < 16) joystick_report_data[1] &= mask;
			else if (button < 24) joystick_report_data[2] &= mask;
			else if (button < 32) joystick_report_data[3] &= mask;
		}
		if (!manual_mode) send_now();
	}

	inline void steering(uint16_t val) {
		if (val > 1023) val = 1023;
		//joystick_report_data[4] = (joystick_report_data[4] & 0x0F) | (val << 4);
		//joystick_report_data[5] = (joystick_report_data[5] & 0xC0) | (val >> 4);
		//joystick_report_data[0] = 0xFF | (val << 4);
		//joystick_report_data[1] = (val >> 4);
		joystick_report_data[0] = val & 0xFF;
		joystick_report_data[1] = ((joystick_report_data[1] & 0xFC) | (val >> 8)) & 0xFF;
		if (!manual_mode) send_now();
	}

	inline void throttle(uint16_t val) {
		if (val > 1023) val = 1023;
		//joystick_report_data[1] = (joystick_report_data[5] & 0x3F) | (val << 6);
		//joystick_report_data[2] = (val >> 2);
		joystick_report_data[1] = ((joystick_report_data[1] & 0x03) | (val << 2)) & 0xFF;
		joystick_report_data[2] = ((joystick_report_data[2] & 0xF0) | (val >> 6)) & 0xFF;
		if (!manual_mode) send_now();
	}

	inline void brake(uint16_t val) {
		if (val > 1023) val = 1023;
		joystick_report_data[2] = ((joystick_report_data[2] & 0x0F) | (val << 4)) & 0xFF;
		joystick_report_data[3] = ((joystick_report_data[3] & 0xC0) | (val >> 4)) & 0xFF;

		if (!manual_mode) send_now();
	}

	inline void buttonPress(uint16_t b, bool val) {

		// TODO: implement switch
		if (b == 0) {
			joystick_report_data[3] = ((joystick_report_data[3] & 0xBF) | (val << 6));
		}
		if (b == 1) {
			joystick_report_data[3] = ((joystick_report_data[3] & 0x7F) | (val << 7));
		}
		if (b == 2) {
			joystick_report_data[4] = ((joystick_report_data[4] & 0xFE) | (val));
		}
		if (b == 3) {
			joystick_report_data[4] = ((joystick_report_data[4] & 0xFD) | (val << 1));
		}
		

		if (!manual_mode) send_now();
	}
	inline void useManualSend(bool mode) {
		manual_mode = mode;
	}
	void send_now(void);
	private:
	//static uint8_t manual_mode;
	uint8_t manual_mode;
};

extern usb_joystick_class DrivingSim;




class usb_serial_class : public Stream
{
	public:
	// standard Arduino functions
	void begin(long);
	void end();
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();
	virtual size_t write(uint8_t);
	using Print::write;
	operator bool();
	// Teensy extensions
	void send_now(void);
	uint32_t baud(void);
	uint8_t stopbits(void);
	uint8_t paritytype(void);
	uint8_t numbits(void);
	uint8_t dtr(void);
	uint8_t rts(void);
	private:
	uint8_t readnext(void);
};

extern usb_serial_class Serial;


#endif
