#ifndef _RFID_H
#define _RFID_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class RFID {
	public:
		RFID();
		void initialize();
		void begin(int pin_D0, int interrupt_D0, int pin_D1, int interrupt_D1);
		bool is_available();
		unsigned long get_rfid_id();
	private:
		static void read_D0();
		static void read_D1();
		static bool convert();
		static unsigned long get_card_id(volatile unsigned long *code_high, volatile unsigned long *code_low, char bit_length);
	
		static volatile unsigned long 	_card_id_temp_high;
		static volatile unsigned long 	_card_id_temp;
		static volatile unsigned long 	_last_read;
		static volatile int	_bit_counter;	
		static unsigned long	_card_id_read;
};	

#endif
