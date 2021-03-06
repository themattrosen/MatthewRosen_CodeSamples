/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   EventListener.h - v1.0
	Author: Matthew Rosen

	Summary:
		Prototype of EventListener type. Includes ability to subscribe
        to different EventTypes.

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#pragma once

// includes
#include "EventInclude.h"
#include <unordered_set>

// base class for objects that want to listen to events sent out of certain types
class EventListener
{
public:
    // default ctor, copy ctor, and virtual dtor
	EventListener() = default;
	EventListener(const EventListener& rhs) = default;
    virtual ~EventListener();


	// inheritors must override this:
	// called when an event that this is subscribed to is sent out
    virtual void OnEvent(Event* ev) = 0;

    // call subscribe in derived class to subscribe to specific EventTypes. 
    void Subscribe(EventType type);

    // call unsuscribe at any time to stop listening for an EventType
    void Unsubscribe(EventType type);

    // call Unsubscribe in derived class exit in order to stop listening for events.
    void UnsubscribeAll();

private:
    std::unordered_set<EventType> m_subscribeList;  // list of EventTypes subscribed to
};

// helper macro to safely cast an event to a derived type
#define CAST_TO_EVENT(EVENT_PTR, NEW_EVENT_TYPE) dynamic_cast<NEW_EVENT_TYPE *>(EVENT_PTR)

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
