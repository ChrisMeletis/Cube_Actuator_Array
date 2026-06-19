#ifndef COMMANDSEXECUTIONERBASE_H
#define COMMANDSEXECUTIONERBASE_H

#define COMMANDSEXECUTIONERBASE_VERSION 0

#include "base.h"


/* REQUIREMENTS */
#define REQUIRED_BASE_VERSION 0

#if BASE_VERSION < REQUIRED_BASE_VERSION
#error base.h version requirement not satisfied
#endif

#ifndef MAX_COMMAND_LENGTH
#define MAX_COMMAND_LENGTH 8 // Length of each command in characters
#endif
#ifndef MAX_HELP_TEXT_LENGTH
#define MAX_HELP_TEXT_LENGTH 2
#endif
#ifndef MAX_COMMANDS
#define MAX_COMMANDS 15		// Max number of commands we are able to add
#endif

// Struct holding a command and its corresponding function 
typedef struct _command_to_function_array
{
	char command[MAX_COMMAND_LENGTH] = {0};
	void (*function)() = NULL;
	// uint8_t number_of_arguments;
	char help_text[MAX_HELP_TEXT_LENGTH] = {0};
}CommandStruct;

// Class declaration
class CommandsExecutionerBase : public Base
{
public:
	// Functions
	bool execute(const char* command_to_execute);
	bool add_command(const char* command, void(*function)(), const char* help_text);
	// bool add_command(const char* command, void(*function)(), uint8_t number_of_arguments);
	char* get_help_text_for_command(const char* command_name);
	void get_help_text_for_all_commands(char* buffer);
	
private:
	// Data
	CommandStruct commands_list[MAX_COMMANDS];
	uint8_t commands_list_index = 0;
		
	// Functions
	bool is_commands_list_full();
	int8_t get_command_id(const char* command_name);
};


/* PUBLIC */


// Function that adds a new command/function pair to the list of available commands
bool CommandsExecutionerBase::add_command(const char* command, void(*function)(), const char* help_text = "")
{
	if(!is_commands_list_full())
	{
		if(strlen(command) > MAX_COMMAND_LENGTH - 1)
		{
			debug.print(F("Error:command name must be up to"));
			debug.print((uint8_t) (MAX_COMMAND_LENGTH - 1));
			debug.println();
			return false;
		}
		if(strlen(help_text) > MAX_HELP_TEXT_LENGTH - 1)
		{
			debug.print(F("Warning:help text longer than "));
			debug.print((uint8_t) (MAX_HELP_TEXT_LENGTH - 1));
			debug.println();
		}
		// Add new command to list
		strncpy(commands_list[commands_list_index].command, command, MAX_COMMAND_LENGTH);
		commands_list[commands_list_index].command[MAX_COMMAND_LENGTH-1] = '\0'; // Finalize string

		commands_list[commands_list_index].function = function;

		strncpy(commands_list[commands_list_index].help_text, help_text, MAX_HELP_TEXT_LENGTH-1);
		commands_list[commands_list_index].help_text[MAX_HELP_TEXT_LENGTH-1] = '\0'; // Finalize string
		// commands_list[commands_list_index].number_of_arguments = number_of_arguments;

		// debug.print(F("Added \""));
		// debug.print(commands_list[commands_list_index].command);
		// debug.print(F("\" to commands_list\r\n"));
		commands_list_index++;
		return true;
	}
	debug.print(F("commands_list full!\r\n"));
	return false;
}

// Function that looks for the supplied command in the list of available commands
// and executes the corresponding function.
// Returns true if command was found, else returns false.
bool CommandsExecutionerBase::execute(const char* command_to_execute)
{
	// debug.print(F("Executing command: "));
	// debug.print(command_to_execute);
	// debug.println();

	int8_t command_id = get_command_id(command_to_execute);
	if(command_id >= 0)
	{
		debug.print(F("Running command: "));
		debug.print(command_id);
		debug.println();
		(*commands_list[command_id].function)();
		return true;
	}
	return false;
}

char* CommandsExecutionerBase::get_help_text_for_command(const char* command_name)
{
	int8_t command_id = get_command_id(command_name);
	if(command_id >= 0)
	{
		return commands_list[command_id].help_text;
	}
	return NULL;
}

// Function that fills the supplied buffer with the help text of
// all the available commands
// Does NOT check for buffer length #TODO
void CommandsExecutionerBase::get_help_text_for_all_commands(char* buffer)
{
	strcpy(buffer, "AVAILABLE COMMANDS:\r\n");
	strcat(buffer, "INDEX\t  COMMAND\tDESCRIPTION\r\n");
	for(int i = 0; i < commands_list_index; i++)
	{
		char index[3] = "";
		sprintf(index, "%02d", i+1);
		strcat(buffer, index);
		strcat(buffer, ":\t");
		char command[10] = "";
		sprintf(command, "%9s", commands_list[i].command);
		strcat(buffer, command);
		strcat(buffer, "\t");
		strcat(buffer, commands_list[i].help_text);
		strcat(buffer, "\r\n");
	}
	
}
/* PRIVATE */

// Function that returns true if the list of commands is full
bool CommandsExecutionerBase::is_commands_list_full()
{
	return(commands_list_index >= MAX_COMMANDS);
}

int8_t CommandsExecutionerBase::get_command_id(const char* command_name)
{
	int8_t command_id = -1;
	for(int i = 0; i < commands_list_index; i++)
	{
		if(strcmp(commands_list[i].command, command_name) == 0)
		{
			// debug.print(F("Found command in list."));
			// debug.println();
			command_id = i;
			break;
		}
	}
	if(command_id < 0)
	{
		debug.print(F("Command nof found in list."));
		debug.println();
	}
	return command_id;
}

#endif
