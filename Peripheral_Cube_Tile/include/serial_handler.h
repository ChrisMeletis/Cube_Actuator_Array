#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include "base.h"
#include "tools.h"

#ifndef MAX_MESSAGE_LENGTH
#define MAX_MESSAGE_LENGTH 20
#endif

#ifndef MAX_MESSAGE_TERMINATORS
#define MAX_MESSAGE_TERMINATORS 3
#endif

/**
   Class that wraps around Arduino Serial class and provides 
   basic handling of incoming commands.
	
   -Has a buffer of length 1, meaning that once a message has been received, 
   all new messages are not processed until the message is cleared by using
   get_message method.
	
   Example usage:
	
   1) 	In setup(), call the start() method like this: a.start(&Serial, 9600)
   2) 	In every loop(), call the listen() method and directly below the has_buffered_message(),
   to verify if there is a message waiting.
   3)	Get the message by calling get_message(<char array to copy the message to>).
	
	
   TODO
   --------------------
   - Change length of buffer
   - Buffer more than one messages?
   

   --------------------
	
   SerialHandler main_serial = SerialHandler();
	
   void setup()
   {
   main_serial.start(&Serial);
   }
	
   void loop()
   {
   main_serial.listen();
   if(main_serial.has_message())
   {
   char buffer[20];
   main_serial.get_message(buffer);
   Serial.println(buffer);
   }
   }
*/

// Class declaration
class SerialHandler : public Base
{
public:
	// Functions
	SerialHandler(){serial = NULL;}  // Constructor
	void start(TinyDebugSerial* _serial, bool use_checksum, long baudrate);  // Initialization
	void listen();  // To be called on every loop, fills buffer
	bool has_message();  // True if there is a message waiting
	void get_message(char* buffer_to_fill);  // Fills the supplied array with the buffered message

	void add_terminator(const char terminator);
	
private:
	// Data
	TinyDebugSerial* serial = NULL;  // pointer to serial to be used to listen
	uint8_t buffer_index = 0;  // variable which keeps track of position in serial_buffer
	bool has_buffered_message = false;  // variable which determines if serial_buffer has a message
	char serial_buffer[MAX_MESSAGE_LENGTH] = {0};  // serial_buffer for temp storage of messages
	bool is_checksum_enabled = false;
	char message_terminators[MAX_MESSAGE_TERMINATORS] = {0};
	uint8_t current_number_of_terminators = 0;
	
	// Functions
	bool is_buffer_full();  // function which determines if serial_buffer is full
	void terminate_buffer();  // Terminate the buffer
	void clear();  // Reset SerialHandler serial_buffer
	bool is_message_terminator(char character);
};

/** PUBLIC FUNCTIONS */
void SerialHandler::get_message(char* buffer_to_fill)
{
	strncpy(buffer_to_fill, serial_buffer, MAX_MESSAGE_LENGTH);
	clear();
}

void SerialHandler::start(TinyDebugSerial* _serial, bool use_checksum = false, long baudrate = 9600)
{
	if(_serial == NULL)
		return;
	clear();
	is_checksum_enabled = use_checksum;
	serial = _serial;
	serial->end();
	serial->begin(baudrate);

	if(current_number_of_terminators == 0)
	{
		add_terminator('\r');
		add_terminator('\n');
	}
}

// Function that listens to specified serial port for messages.
// If a complete message has arrived, it stops listening until the 
// buffer is cleared (using get_message).
void SerialHandler::listen()
{
	// Do not read serial if we have a message ready
	if(serial == NULL || has_buffered_message) 
	{
		return;
	}
	
	// We do not have a complete buffered message
	while(serial->available())
	{
		char c = serial->read();
		
		// Determine if message should be terminated
		if(is_message_terminator(c))
		{
			terminate_buffer();
		}
		else if(c>0)
		{
			// Append character to buffer
			serial_buffer[buffer_index++] = c;
			
			// Check if buffer full
			if(is_buffer_full())
			{
				terminate_buffer();
			}
		}
	}	
}

void SerialHandler::add_terminator(const char terminator)
{
	if(terminator < 0)
	{
		debug.print(F("Invalid terminator "));
		debug.print((uint8_t) terminator);
		debug.print(F("\r\n"));
		return;
	}
	if(current_number_of_terminators >= MAX_MESSAGE_TERMINATORS)
	{
		debug.print(F("Number of message terminators exceeded."));
		return;
	}
	
	message_terminators[current_number_of_terminators] = terminator;
	debug.print(F("Successfully added terminator "));
	debug.print((uint8_t) terminator);
	debug.print(F("\r\n"));
	current_number_of_terminators++;
}

/** PRIVATE FUNCTIONS */

bool SerialHandler::is_buffer_full()
{
	return(buffer_index >= MAX_MESSAGE_LENGTH - 1);
}

void SerialHandler::terminate_buffer()
{
	// This is to skip single CR or LF characters
	if(serial_buffer[0] =='\0')
	{
		return;
	}
	// Terminate buffer
	serial_buffer[buffer_index] = '\0';

	// Check checksum, if enabled
	if(is_checksum_enabled && !is_checksum_ok((uint8_t*) serial_buffer, buffer_index+1))
	{
		debug.print(F("Checksum failed for message \""));
		debug.print(serial_buffer);
		debug.print(F("\"\r\n"));
		clear();
		return;
	}
	debug.print(F("Got message: \""));
	debug.print(serial_buffer);
	debug.print(F("\"\r\n"));
	
	// Raise flag
	has_buffered_message = true;
}


// Returns whether the handler has a buffered message
bool SerialHandler::has_message()
{
	return has_buffered_message;
}

// Clear the buffer of the handler
void SerialHandler::clear()
{
	serial_buffer[0] = '\0';
	buffer_index = 0;
	has_buffered_message = false;
}

bool SerialHandler::is_message_terminator(char character)
{
	for(int i = 0; i < current_number_of_terminators; i++)
	{
		if(character == message_terminators[i])
		{
			return true;
		}
	}
	return false;
}

#endif
