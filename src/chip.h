#include "Arduino.h"
#include "pins_arduino.h"

#ifndef __CHIP__
	#define __CHIP__

	#if defined(ARDUINO_ARCH_ESP8266) //ESP8266
		static const char* arduino_arch = "esp8266";
	#elif defined(ARDUINO_ARCH_ESP32) //ESP32
		static const char* arduino_arch = "esp32";
	#elif defined(ARDUINO_ARCH_AVR)
		static const char* arduino_arch = "avr";
	#elif defined(ARDUINO_ARCH_SAM) //Arduino Due
		static const char* arduino_arch = "sam";
	#else
		static const char* arduino_arch = "unknown";
	#endif

	static const uint8_t analog_input_map[] = {
	#if defined(ARDUINO_ARCH_ESP32)
		A0,
		A3,
		A4,
		A5,
		A6,
		A7,
		A10,
		A11,
		A12,
		A13,
		A14,
		A15,
		A16,
		A17,
		A18,
		A19
	#else
		#if (NUM_ANALOG_INPUTS>0)
			A0
		#endif
		#if (NUM_ANALOG_INPUTS>1)
			,A1
		#endif
		#if (NUM_ANALOG_INPUTS>2)
			,A2
		#endif
		#if (NUM_ANALOG_INPUTS>3)
			,A3
		#endif
		#if (NUM_ANALOG_INPUTS>4)
			,A4
		#endif
		#if (NUM_ANALOG_INPUTS>5)
			,A5
		#endif
		#if (NUM_ANALOG_INPUTS>6)
			,A6
		#endif
		#if (NUM_ANALOG_INPUTS>7)
			,A7
		#endif
		#if (NUM_ANALOG_INPUTS>8)
			,A8
		#endif
		#if (NUM_ANALOG_INPUTS>9)
			,A9
		#endif
		#if (NUM_ANALOG_INPUTS>10)
			,A10
		#endif
		#if (NUM_ANALOG_INPUTS>11)
			,A11
		#endif
		#if (NUM_ANALOG_INPUTS>12)
			,A12
		#endif
		#if (NUM_ANALOG_INPUTS>13)
			,A13
		#endif
		#if (NUM_ANALOG_INPUTS>14)
			,A14
		#endif
		#if (NUM_ANALOG_INPUTS>15)
			,A15
		#endif
		#if (NUM_ANALOG_INPUTS>16)
			,A16
		#endif
		#if (NUM_ANALOG_INPUTS>17)
			,A17
		#endif
		#if (NUM_ANALOG_INPUTS>18)
			,A18
		#endif
		#if (NUM_ANALOG_INPUTS>19)
			,A19
		#endif
		#if (NUM_ANALOG_INPUTS>20)
			,A20
		#endif
		#if (NUM_ANALOG_INPUTS>21)
			,A21
		#endif
		#if (NUM_ANALOG_INPUTS>22)
			,A22
		#endif
		#if (NUM_ANALOG_INPUTS>23)
			,A23
		#endif
		#if (NUM_ANALOG_INPUTS>24)
			,A24
		#endif
		#if (NUM_ANALOG_INPUTS>25)
			,A25
		#endif
		#if (NUM_ANALOG_INPUTS>26)
			,A26
		#endif
		#if (NUM_ANALOG_INPUTS>27)
			,A27
		#endif
		#if (NUM_ANALOG_INPUTS>28)
			,A28
		#endif
		#if (NUM_ANALOG_INPUTS>29)
			,A29
		#endif
		#if (NUM_ANALOG_INPUTS>30)
			,A30
		#endif
		#if (NUM_ANALOG_INPUTS>31)
			,A31
		#endif
	#endif
	};

  static const uint8_t digital_pin_map[] = {
	#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    D0, D1, D2, D3, D4, D5, D6, D7, D8
	#elif defined(ARDUINO_ESP8266_NODEMCU)
	  D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10
	#else
		#ifdef D0
			D0
		#endif
		#ifdef D1
			,D1
		#endif
		#ifdef D2
			,D2
		#endif
		#ifdef D3
			,D3
		#endif
		#ifdef D4
			,D4
		#endif
		#ifdef D5
			,D5
		#endif
		#ifdef D6
			,D6
		#endif
		#ifdef D7
			,D7
		#endif
		#ifdef D8
			,D8
		#endif
		#ifdef D9
			,D9
		#endif
		#ifdef D10
			,D10
		#endif
		#ifdef D11
			,D11
		#endif
		#ifdef D12
			,D12
		#endif
		#ifdef D13
			,D13
		#endif
		#ifdef D14
			,D14
		#endif
		#ifdef D15
			,D15
		#endif
		#ifdef D16
			,D16
		#endif
		#ifdef D17
			,D17
		#endif
		#ifdef D18
			,D18
		#endif
		#ifdef D19
			,D19
		#endif
		#ifdef D20
			,D20
		#endif
		#ifdef D21
			,D21
		#endif
		#ifdef D22
			,D22
		#endif
		#ifdef D23
			,D23
		#endif
		#ifdef D24
			,D24
		#endif
		#ifdef D25
			,D25
		#endif
		#ifdef D26
			,D26
		#endif
		#ifdef D27
			,D27
		#endif
		#ifdef D28
			,D28
		#endif
		#ifdef D29
			,D29
		#endif
		#ifdef D30
			,D30
		#endif
		#ifdef D31
			,D31
		#endif
	#endif
	};

#endif
