#ifndef BASE_H
#define BASE_H
#define BASE_VERSION 0

#include <Arduino.h>
#include "debugger.h"

/* REQUIREMENTS */
#define REQUIRED_DEBUGGER_VERSION 0

#if DEBUGGER_VERSION < REQUIRED_DEBUGGER_VERSION
#error debugger.h version requirement not satisfied
#endif

/* 
   Base class to be used for other classes that need
   basic fucntionality like debugging and timing. 
*/
class Base
{
// protected:

public:
	// If debug is made a static (class) variable, which is shared with all instances,
	// some program storage is saved but we lose the ability to enable/disable the debugger
	// selectively on each inherited class.
	// To make it a class variable, shared among all instances:
	// static Debugger debug;
	// and outside of the class declaration:
	// Debugger Base::debug;
	Debugger debug;
	inline Base();
	inline void enable_debugging(TinyDebugSerial * _serial);
	inline void disable_debugging();
	inline bool is_debugger_enabled();
	inline void enable();
	inline void disable();
	inline bool is_enabled();

private:
	bool _is_enabled:1;
};

inline Base::Base()
{
	_is_enabled = true;
}

inline bool Base::is_enabled()
{
	return _is_enabled;
}

inline void Base::enable()
{
	_is_enabled = true;
}

inline void Base::disable()
{
	_is_enabled = false;
}

inline bool Base::is_debugger_enabled()
{
	return debug.is_debugger_enabled();
}

inline void Base::enable_debugging(TinyDebugSerial* _serial)
{
	if(_serial == NULL)
		return;
	debug.enable(_serial);
}

inline void Base::disable_debugging()
{
	debug.disable();
}


#endif
