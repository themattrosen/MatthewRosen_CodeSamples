/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   EventInclude.h - v1.0
	Author: Matthew Rosen

	Summary:
		Defines Event data: EventTypes, Event name strings, and Event class encapsulations.

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#pragma once

// includes
#include "Util.h"

// define EVENT to be enum naming convention with comma
#define EVENT(x, ...) et##x##Event ,

// enum of event types 
enum EventType
{
    // inline include the registry with entries defined above
#   include "EventRegistry.h"
    
    etEventMax
};

// extern global array of event names
extern const char* g_EventNames[];

// base class of events
struct Event
{
    // explicit conversion ctor: 
    // event defined by EventType
    explicit Event(EventType type)
        : m_type(type) {}

    // virtual clone function to return dynamically alloced copy of self
    virtual Event* Clone() = 0;

    // virtual ToString to retrieve name from g_EventNames
	virtual const char* ToString() = 0;

    EventType m_type;   // internal type of the Event
};

// helper macro to separate the __VA_ARGS__ of EVENT defines
#define SEPARATE_ARGS(arg) arg ;

// disable obscure warnings
#pragma warning( push )  
#pragma warning( disable : 4003 )  

// redefine EVENT to be a struct inheriting from Event that overrides the Clone and ToString methods
// expands __VA_ARGS__ to be separated by semicolons, allowing the struct to contain data
#undef EVENT
#define EVENT(x, ...)                           \
struct x##Event : public Event                  \
{                                               \
    x##Event() : Event(et##x##Event) {}     \
    x##Event(const x##Event& rhs) = default;    \
    Event* Clone() { return new x##Event(*this); }   \
	const char* ToString() { return g_EventNames[static_cast<int>(m_type)]; }	\
    APPLY_ON_EACH(SEPARATE_ARGS, __VA_ARGS__)   \
};

// inline include the registry to create all the event class definitions
#include "EventRegistry.h"

#pragma warning( pop )

// redefine EVENT to be nothing for safety
#undef EVENT
#define EVENT

/*
	------------------------------------------------------------------------------
	This software is available under 2 licenses - you may choose the one you like.
	------------------------------------------------------------------------------
	ALTERNATIVE A - zlib license
	Copyright (c) 2019 Matthew Rosen
	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from
	the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	  1. The origin of this software must not be misrepresented; you must not
		 claim that you wrote the original software. If you use this software
		 in a product, an acknowledgment in the product documentation would be
		 appreciated but is not required.
	  2. Altered source versions must be plainly marked as such, and must not
		 be misrepresented as being the original software.
	  3. This notice may not be removed or altered from any source distribution.
	------------------------------------------------------------------------------
	ALTERNATIVE B - Public Domain (www.unlicense.org)
	This is free and unencumbered software released into the public domain.
	Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
	software, either in source code form or as a compiled binary, for any purpose,
	commercial or non-commercial, and by any means.
	In jurisdictions that recognize copyright laws, the author or authors of this
	software dedicate any and all copyright interest in the software to the public
	domain. We make this dedication for the benefit of the public at large and to
	the detriment of our heirs and successors. We intend this dedication to be an
	overt act of relinquishment in perpetuity of all present and future rights to
	this software under copyright law.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
	ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	------------------------------------------------------------------------------
*/
