#include "Arduino.h"
#include "pins_arduino.h"

#ifndef CHIP
	#if defined(__AVR_ATmega328P__) // uno, fio
		#define CHIP "atmega328p"
	#elif defined(__AVR_ATSAMD21G18A__) // zero
		#define CHIP "atsamd21g18a"
	#elif defined(__AVR_ATSAM3X8E__) // Due
		#define CHIP "atsam3x8e"
	#elif defined(__AVR_Atmega32U4__) // Yun 16Mhz, Micro, Leonardo, Esplora
		#define CHIP "atmega32u4"
	#elif defined(_AVR_AR9331__) // Yun 400Mhz
		#define CHIP "ar9331"
	#elif defined(__AVR_ATmega16U4__) // leonardo
		#define CHIP "atmega16u4"
	#elif defined(__AVR_ATmega1280__) // mega, Mega ADK
		#define CHIP "atmega1280"
	#elif defined(__AVR_ATmega2560__) // mega, Mega ADK
		#define CHIP "atmega2560"
	#elif defined(_AVR_ATmega328__) // Nano >= v3.0 or Arduino Pro, pro328, ethernet
		#define CHIP "atmega328"
	#elif defined(_AVR_ATmega168__) // Nano < v3.0 or uno, pro
		#define CHIP "atmega168"
	#elif defined(_AVR_ATmega168V__) // LilyPad
		#define CHIP "atmega168v"
	#elif defined(_AVR_ATmega328V__) // LilyPad 2
		#define CHIP "atmega328v"
	#elif defined(_AVR_ATTiny85__) // trinket
		#define CHIP "attiny85"
	#elif  defined(__AVR_ARCv2EM__) // Intel Curie/101
		#define CHIP "arcv2em"
	#elif defined(__CURIE_FACTORY_DATA_H_) // Intel Curie/101
		#define CHIP "curiefactorydata"
	#elif defined(ARDUINO_ARCH_ESP8266) //ESP8266
		#define CHIP "esp8266"
	#elif defined(ARDUINO_ARCH_ESP32) //ESP32
		#define CHIP "esp32"
	#else
		#define CHIP "unknown"
	#endif
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
};

