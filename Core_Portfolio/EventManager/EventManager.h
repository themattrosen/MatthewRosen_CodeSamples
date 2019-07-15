/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   EventManager.h - v1.0
	Author: Matthew Rosen

	Summary:
		Prototype of EventManager singleton. 
        Includes functionality for subscribing event listeners to event types,
        send events immediately, and queueing events for dispatch later, either
        at the end of the frame or after a set amount of time.

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#pragma once

// includes
#include <unordered_set>
#include <vector>
#include <queue>
#include <deque>
#include "Singleton.h"

// forward declarations
struct Event;
class EventListener;

// global EventManager singleton
class EventManager : public Singleton<EventManager>
{
public:
    // destructor is default, because memory is handled in Init and Exit
    ~EventManager() = default;

    // Init internal memory, called on applications startup
    void Init();

    // update to send out events queued for a specific time
    void Update(float dt);

    // send events queued for end of frame
    void LateUpdate();

    // delete allocated memory, called on application end
    void Exit();

    // send address of local event
    // usage:   EventName ev;
    //          EventManager::Instance().SendEvent(&ev);
    void SendEvent(Event* ev);

    // send dynamically allocated event at the end of the frame
    // can delay sending events for a specific amount of seconds
    // uses new/delete, avoid usage
    // usage (queueing for late update):
    //      EventManager::Instance().QueueEvent(new EventName);
    // 
    // usage (queue for sending after set amount of time): 
    //      EventManager::Instance().QueueEvent(new EventName, amtTimeDelayed);
    void QueueEvent(Event* ev, float timeDelay = 0.f);

    // unsubscribe a listener from all events
    void RemoveListener(EventListener* listener);

private:
    // default ctor private to enforce singleton pattern
    EventManager() = default;

    // friend classes
    friend class Singleton<EventManager>;   // singleton friend for default ctor
    friend class EventListener;             // EventListener friend for private versions of subscribe/unsubscribe

    // private subscribe/unsubscribe used by the event listener
    void Subscribe(EventListener* listener, int eventType);
    void Unsubscribe(EventListener* listener, int eventType);

    // holds event listeners paired with the events they're listening to
    // index of array = EventType as int
    std::vector<std::unordered_set<EventListener *>> m_eventListeners;

    // holds event ptrs queued to be sent at the end of the frame
    std::queue<Event *> m_queuedEvents;

    // holds events ptrs that are to be sent after a set amount of time
    std::deque<std::pair<Event *, float>> m_delayedEvents;
};

// helper macros to send events from any file
#define SEND_EVENT(ev) EventManager::Instance().SendEvent(&ev)
#define QUEUE_EVENT(ev) EventManager::Instance().QueueEvent(ev.Clone())
#define DELAY_EVENT(ev, duration) EventManager::Instance().QueueEvent(ev.Clone(), duration)

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
