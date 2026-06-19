#ifndef COMMANDSWRAPPERCLASS_H
#define COMMANDSWRAPPERCLASS_H

#define REQUIRED_COMMANDSEXECUTIONERBASE_VERSION 0
#define REQUIRED_COMMANDSQUEUEHANDLER_VERSION 0

#include "commands_executioner_base.h"
#include "commands_queue_handler.h"


/* REQUIREMENTS */

#if COMMANDSEXECUTIONERBASE_VERSION < REQUIRED_COMMANDSEXECUTIONERBASE_VERSION
#error commands_executioner_base.h version requirement not satisfied
#endif

#if COMMANDSQUEUEHANDLER_VERSION < REQUIRED_COMMANDSQUEUEHANDLER_VERSION
#error commands_queue_handler.h version requirement not satisfied
#endif

/* 
   Wrapper class inheriting from the following two classes:
   - CommandsExecutionerBase
   - CommandsQueueHandler
	
   The idea is to only have one class handling the commands queue and execution.
	
   Apart from that, no special functionality is added.
   Check declarations of CommandsExecutionerBase and CommandsQueueHandler
   for a list of available methods.
	
*/

// Class declaration
class CommandsWrapperClass : public CommandsExecutionerBase, public CommandsQueueHandler
{
public:
	void enable_debugging(TinyDebugSerial* _serial);	 // Override enable_debugging() to activate it on both inherited classes
	void disable_debugging();

	void enable();
	void disable();
	bool is_enabled();
		
};

void CommandsWrapperClass::enable()
{
	CommandsExecutionerBase::enable();
}

void CommandsWrapperClass::disable()
{
	CommandsExecutionerBase::disable();
}

bool CommandsWrapperClass::is_enabled()
{
	return CommandsExecutionerBase::is_enabled();
}

void CommandsWrapperClass::enable_debugging(TinyDebugSerial* _serial)
{
	CommandsExecutionerBase::enable_debugging(_serial);
	CommandsQueueHandler::enable_debugging(_serial);
}

void CommandsWrapperClass::disable_debugging()
{
	CommandsExecutionerBase::disable_debugging();
	CommandsQueueHandler::disable_debugging();
}	

#endif
