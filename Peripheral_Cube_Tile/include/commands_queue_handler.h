#ifndef COMMANDSQUEUEHANDLER_H
#define COMMANDSQUEUEHANDLER_H

#define COMMANDSQUEUEHANDLER_VERSION 0

#include "base.h"

/* REQUIREMENTS */
#define REQUIRED_BASE_VERSION 0

#if BASE_VERSION < REQUIRED_BASE_VERSION
#error base.h version requirement not satisfied
#endif

#ifndef MAX_COMMANDS_QUEUE_LENGTH
#define MAX_COMMANDS_QUEUE_LENGTH 	6  // Number of commands/arguments the queue is able to hold
#endif
#ifndef MAX_COMMAND_LENGTH
#define MAX_COMMAND_LENGTH 	15  // Max length (in characters) for each command/argument
#endif
// #if MAX_COMMAND_LENGTH > MAX_COMMAND_LENGTH
// #undef MAX_COMMAND_LENGTH
// #define MAX_COMMAND_LENGTH MAX_COMMAND_LENGTH
// #endif


/**
   Simple circular FIFO queue.
	
   Uses two indices:
	
   commands_queue_current_request_index: 	Index of the next command to be executed.
   commands_queue_current_storage_index:	Index of the position where the next command 
   will be stored in.
	
   ---------------------------EXAMPLE------------------------------
   #include "serial_handler.h"
   #include "commands_queue_handler.h"
	
   SerialHandler main_serial = SerialHandler();
   CommandsQueueHandler commands_queue = CommandsQueueHandler();
	
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
			
   if(!commands_queue.process_command(buffer))
   {
   commands_queue.clear();
   commands_queue.process_command(buffer);
   }
   }
   }
*/


// Class declaration
class CommandsQueueHandler : public Base
{
public:
	// Constructor
	CommandsQueueHandler();
		
	// Functions
	bool process_command(char* message);
	void get_command(char* buffer_to_fill);
	bool has_commands();
	void clear();
	uint8_t print_contents();
	void extract_arguments(float* arg_list, uint8_t number_of_args);
	void extract_arguments(double* arg_list, uint8_t number_of_args);
	void extract_arguments(char arg_list[][MAX_COMMAND_LENGTH], uint8_t number_of_args);
	void extract_arguments(char* arg_list, uint8_t number_of_args);
	void extract_arguments(int* arg_list, uint8_t number_of_args);
	void extract_arguments(uint8_t* arg_list, uint8_t number_of_args);
	// void extract_arguments(char** arg_list, uint8_t number_of_args);

	
private:
	// Data
	char commands_queue[MAX_COMMANDS_QUEUE_LENGTH][MAX_COMMAND_LENGTH];
	int8_t commands_queue_current_storage_index = 0; // Index in commands_queue where the next command will be placed in
	int8_t commands_queue_current_request_index = 0; // Index of last command requested
	uint8_t commands_queue_length = MAX_COMMANDS_QUEUE_LENGTH;
	char token_seperator[2] = {',', '\0'};

	// Functions
	bool is_queue_full();
	void remove_command();
	bool append_command(const char* command);
	
};

// Constructor
CommandsQueueHandler::CommandsQueueHandler()
{
	clear();
}


/** Public methods */
// Tokenize supplied command by splitting it using the provided delimiter
// After the command has been tokenized, each token is stored as
// a separate command. 
bool CommandsQueueHandler::process_command(char* message)
{
	// debug.print(F("Splitting \""));
	// debug.print(message);
	// debug.print(F("\" with \""));
	// debug.print(token_seperator);
	// debug.print(F("\"\r\n"));
	char* token;
	token = strtok(message, token_seperator);
	
	while(token != NULL)
	{
		// debug.print(F("Token:"));
		// debug.print(token);
		// debug.print(F("\r\n"));
		if(!append_command(token))
		{
			// Queue is full
			return false;
		}
		token = strtok(NULL, token_seperator);
	}
	// debug.print(F("Finished tokenizing\r\n"));
	return true;
}

void CommandsQueueHandler::remove_command()
{
	if(!has_commands())
	{
		return;
	}
	
	commands_queue[commands_queue_current_request_index][0] = '\0';
	commands_queue_current_request_index++;
	commands_queue_current_request_index %= MAX_COMMANDS_QUEUE_LENGTH;
}

// Iterate through the queue and empty all strings
void CommandsQueueHandler::clear()
{
	// debug.print(F("Clearing CommandsQueue\r\n"));
	
	uint8_t number_of_commands_found = 0;

	for(uint8_t i=0;i<MAX_COMMANDS_QUEUE_LENGTH;i++)
	{
		if(commands_queue[i][0] != '\0')
		{
			number_of_commands_found++;
			commands_queue[i][0]='\0';
		}
	}
	commands_queue_current_request_index = 0;
	commands_queue_current_storage_index=0;
	debug.print(F("Removed "));
	debug.print(number_of_commands_found);
	debug.print(F(" commands.\r\n"));
}

// If there is a command at commands_queue_current_request_index, then the queue
// is not empty.
bool CommandsQueueHandler::has_commands()
{
	// for(int i=0;i<MAX_COMMANDS_QUEUE_LENGTH;i++)
	// {
	// if(commands_queue[i][0] != '\0')
	// {
	// return true;
	// }
	// }
	// return false;
	if(!is_enabled())
	{
		debug.print(F("CommandsQueueHandler disabled"));
		debug.println();
		return false;
	}
	return(commands_queue[commands_queue_current_request_index][0] != '\0');
}

// Get the next command to be executed, copy it to the buffer supplied and
// remove the command from the queue.
void CommandsQueueHandler::get_command(char* buffer_to_fill)
{
	if(has_commands())
	{
		strncpy(buffer_to_fill, commands_queue[commands_queue_current_request_index], MAX_COMMAND_LENGTH);  // Copy command to supplied buffer
		remove_command(); // Remove from queue
		return;
	}
	buffer_to_fill[0] = '\0'; // clear buffer if no room in queue
}

// Function that prints contents of queue in debug serial
uint8_t CommandsQueueHandler::print_contents()
{
	if(!has_commands())
	{
		debug.print(F("CommandsQueue empty!\r\n"));
		return 0;
	}
	debug.print(F("Contents of CommandsQueue:\r\n"));
	uint8_t number_of_commands = 0;
	for(uint8_t i=0;i<MAX_COMMANDS_QUEUE_LENGTH;i++)
	{
		if(commands_queue[i][0] != '\0')
		{
			debug.print(F("Command"));
			debug.print(i);
			debug.print(F(":"));
			number_of_commands++;
			debug.print(commands_queue[i]);
			debug.print(F("\r\n"));
		}
	}
	// debug.print(F("CommandsQueue length:"));
	// debug.print(number_of_commands);
	// debug.print(F("\r\n"));
	return number_of_commands;
}

// Function that extracts arguments from the command queue. 
// Needs an array and a number as inputs, and tries to parse as many arguments as the number
// dictates from the queue, storing them in the array.
void CommandsQueueHandler::extract_arguments(double* arg_list, uint8_t number_of_args)
{
	char buffer[20];
	for(uint8_t i=0;i<number_of_args;i++)
	{
		get_command(buffer);
		arg_list[i] = strtod(buffer, NULL);
	}
}
void CommandsQueueHandler::extract_arguments(float* arg_list, uint8_t number_of_args)
{
	char buffer[MAX_COMMAND_LENGTH];
	for(uint8_t i=0;i<number_of_args;i++)
	{
		get_command(buffer);
		arg_list[i] = atof(buffer);
	}
}

void CommandsQueueHandler::extract_arguments(int* arg_list, uint8_t number_of_args)
{
	char buffer[MAX_COMMAND_LENGTH];
	for(uint8_t i=0;i<number_of_args;i++)
	{
		get_command(buffer);
		arg_list[i] = atoi(buffer);
	}
}

void CommandsQueueHandler::extract_arguments(uint8_t* arg_list, uint8_t number_of_args)
{
	char buffer[MAX_COMMAND_LENGTH];
	for(uint8_t i=0;i<number_of_args;i++)
	{
		get_command(buffer);
		arg_list[i] = atoi(buffer);
	}
}

void CommandsQueueHandler::extract_arguments(char arg_list[][MAX_COMMAND_LENGTH], uint8_t number_of_args)
{
	char buffer[MAX_COMMAND_LENGTH];
	for(uint8_t i = 0; i < number_of_args; i++)
	{
		get_command(buffer);
		strncpy(arg_list[i], buffer, MAX_COMMAND_LENGTH);
	}
}

//extract single character command
void CommandsQueueHandler::extract_arguments(char* arg_list, uint8_t number_of_args)
{
	// char buffer[MAX_COMMAND_LENGTH];
	// for(uint8_t i = 0; i < number_of_args; i++)
	// {
	// 	get_command(buffer);
	// 	strncpy(arg_list, buffer, MAX_COMMAND_LENGTH);
	// }
	get_command(arg_list);
}


/** Private funcs */

// Stores single commands/arguments in the queue.
// This should only be called from process_command().
bool CommandsQueueHandler::append_command(const char* command)
{
	if(!is_queue_full())
	{
		// debug.print(F("New command:"));
		// debug.print(command);
		// debug.print(F("\r\n"));
		
		strncpy(commands_queue[commands_queue_current_storage_index], command, MAX_COMMAND_LENGTH);
		commands_queue_current_storage_index++;
		commands_queue_current_storage_index %= MAX_COMMANDS_QUEUE_LENGTH;
		
		print_contents();
		
		return true;
	}
	debug.print(F("CommandsQueue full!\r\n"));
	return false;
}

// Returns true if there is already a command at commands_queue_current_storage_index
bool CommandsQueueHandler::is_queue_full()
{
	// for(int i=0;i<MAX_COMMANDS_QUEUE_LENGTH;i++)
	// {
	// if(commands_queue[i][0] == '\0')
	// {
	// return false;
	// }
	// }
	// return true;
	return(commands_queue[commands_queue_current_storage_index][0] != '\0');
}

#endif
