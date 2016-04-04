#include "Rfid.h"

volatile unsigned long RFID::_card_id_temp_high = 0;
volatile unsigned long RFID::_card_id_temp = 0;
volatile unsigned long RFID::_last_read = 0;
volatile int RFID::_bit_counter = 0;
unsigned long RFID::_card_id_read = 0;

RFID::RFID()
{
}

void RFID::initialize()
{
	begin(2,digitalPinToInterrupt(2),3,digitalPinToInterrupt(3));
}

unsigned long RFID::get_rfid_id()
{
	unsigned long temp_id = _card_id_read;
	_card_id_read = 0;
	return temp_id;
}

void RFID::begin(int pin_D0, int interrupt_D0, int pin_D1, int interrupt_D1)
{
	_bit_counter = 0;

	pinMode(pin_D0, INPUT);
	pinMode(pin_D1, INPUT);

	attachInterrupt(interrupt_D0, read_D0, FALLING);	// high to low
	attachInterrupt(interrupt_D1, read_D1, FALLING);	// high to low
}

bool RFID::is_available()
{
	bool available;
	noInterrupts();
	available = convert();
	interrupts();
	return available;
}

// read bit set to 0
void RFID::read_D0() 
{
	_bit_counter++; 			// increment which bit we're at

	if(_bit_counter > 31)	// if we are higher than 31 start processing bits
	{
		_card_id_temp_high |= ((0x80000000 & _card_id_temp)>>31);
		_card_id_temp_high <<= 1;
		_card_id_temp <<= 1;
	}
	else
	{
		_card_id_temp <<= 1;
	}

	_last_read = millis();
}

// read bit set to 1
void RFID::read_D1()
{
	_bit_counter++; 			// increment which bit we're at

	if(_bit_counter > 31)	// if we are higher than 31 start processing bits
	{
		_card_id_temp_high |= ((0x80000000 & _card_id_temp)>>31);
		_card_id_temp_high <<= 1;
		_card_id_temp <<= 1;
	}
	else
	{
		_card_id_temp |= 1;  // sets the bit to 1
		_card_id_temp <<= 1;
	}

	_last_read = millis();
}

unsigned long RFID::get_card_id(volatile unsigned long *code_high, volatile unsigned long *code_low, char bit_length)
{
	unsigned long card_id = 0;

	// we are using bit length of 26
	if(bit_length == 26)
	{
		card_id = (*code_low & 0x1FFFFFE) >> 1; // get_card_id by getting the lower bits and shifting right by 1
	}

	return card_id;
}

bool RFID::convert() 
{
	unsigned long card_id;
	unsigned long system_tick = millis();

	// check to see if no more signals coming in after 25ms
	if(system_tick - _last_read > 25)
	{
		_card_id_temp >>= 1;

		if(_bit_counter > 32)
		{
			_card_id_temp_high >>= 1;
		}

		if(_bit_counter == 26)
		{
			card_id = get_card_id(&_card_id_temp_high, &_card_id_temp, _bit_counter);
			_bit_counter = 0;
			_card_id_temp = 0;
			_card_id_temp_high = 0;
			_card_id_read = card_id;
			return true;
		}
	}
	else
	{
		return false;
	}
}
