#ifndef DEBUGGER_H
#define DEBUGGER_H
#include <Arduino.h>
#define DEBUGGER_VERSION 0

class Debugger
{
public:
	Debugger()
	{
		serial = NULL;
	}
		
	inline void print(const char* string);
	inline void print(const __FlashStringHelper *string);
	inline void print(uint32_t number);
	inline void print(uint16_t number);
	inline void print(uint8_t number);
	inline void print(int8_t number);
	inline void print(int16_t number);
	inline void print(char character);
	inline void print(double number);
	inline void print(float number);
	inline void print(bool boolean_value);
	inline void println();	
	inline void enable(TinyDebugSerial* _serial);
	inline void disable();
	inline void enable_flush();
	inline void disable_flush();
	inline bool is_debugger_enabled();

	
private:
	TinyDebugSerial* serial;
	bool is_enabled = false;
	bool wait_for_flush = false;
};

inline bool Debugger::is_debugger_enabled()
{
	return is_enabled;
}

inline void Debugger::enable_flush()
{
	wait_for_flush = true;
}

inline void Debugger::disable_flush()
{
	wait_for_flush = false;
}


inline void Debugger::enable(TinyDebugSerial* _serial)
{
	serial = _serial;
	if (serial == NULL)
	{
		is_enabled = false;
	}
	else
	{
		is_enabled = true;
	}
}

inline void Debugger::disable()
{
	enable(NULL);
}

inline void Debugger::print(float number)
{
	print(double(number));
}
inline void Debugger::print(char _char)
{
	print(&_char);
}
inline void Debugger::print(double number)
{
	if(!is_enabled) return;
	char buffer[10];
	unsigned int decimal_part = (unsigned int)(abs(number * 100)) % 100;
	// Serial.print(decimal_part);
	// Serial.println("   ");
	sprintf(buffer, "%d.%02d", int(number), decimal_part);
	print(buffer);
}

inline void Debugger::print(bool boolean_value)
{
	if(!is_enabled) return;
	print(uint8_t(boolean_value));
}

inline void Debugger::print(uint8_t number)
{
	if(!is_enabled) return;
	print((uint32_t)number);
}

inline void Debugger::print(uint16_t number)
{
	if(!is_enabled) return;
	// char buffer[20];
	// itoa(number, buffer, 10);
	// print(buffer);
	print((uint32_t) number);
}

inline void Debugger::print(uint32_t number)
{
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "%lu", number);
	print(buffer);
}

inline void Debugger::print(int16_t number)
{
  	if(!is_enabled) return;
	char buffer[20];
	itoa(number, buffer, 10);
	print(buffer);
}

inline void Debugger::print(int8_t number)
{
	if(!is_enabled) return;
	print((int16_t)number);
}

inline void Debugger::print(const char* string)
{
	if(!is_enabled) return;
	if(string == NULL || serial == NULL)
		return;
	serial->print(string);
	
	if(wait_for_flush)
	{
		serial->flush();
	}
}
inline void Debugger::print(const __FlashStringHelper *string)
{
	if(!is_enabled) return;
	if(string == NULL || serial == NULL)
		return;
	serial->print(string);
	
	if(wait_for_flush)
	{
		serial->flush();
	}
}

inline void Debugger::println()
{
	print(F("\r\n"));
}
	
#endif
