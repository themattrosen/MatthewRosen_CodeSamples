/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   EventManager.cpp - v1.0
	Author: Matthew Rosen

	Summary:
		Implementation of EventManager singleton. 
        Includes functionality for subscribing event listeners to event types,
        send events immediately, and queueing events for dispatch later, either
        at the end of the frame or after a set amount of time.

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// includes
#include "EventManager.h"
#include "EventListener.h"

// Initializes EventListener storage memory
void EventManager::Init()
{
    // allocate array to max size
    m_eventListeners.resize(EventType::etEventMax);
}

// handle delayed events with dt
void EventManager::Update(float dt)
{
    // iterate over all delayed events
    for (size_t i = 0; i < m_delayedEvents.size(); ++i)
    {
        // decrement the time from the delay
        auto & nextPair = m_delayedEvents[i];
        nextPair.second -= dt;

        // case time to send out event
        if (nextPair.second <= 0.f)
        {
            // queue it to be sent at the end of the frame
            QueueEvent(nextPair.first);

            // delete from delayed list
            m_delayedEvents.erase(m_delayedEvents.begin() + i);
            --i;
        }
    }
}

// handle events queued to end of frame
void EventManager::LateUpdate()
{
    // process all queued events
    while (!m_queuedEvents.empty())
    {
        // get the next event in the queue
        Event* nextEvent = m_queuedEvents.front();
        m_queuedEvents.pop();

        // dispatch it
        SendEvent(nextEvent);

        // delete the dynamically allocated data
        delete nextEvent;
    }

}

// release memory and exit the EventManager
void EventManager::Exit()
{
    // process each queued event: they won't be sent because of EventManager shutdown
    while (!m_queuedEvents.empty())
    {
        // retrieve and delete memory
        Event* nextEvent = m_queuedEvents.front();
        delete nextEvent;
        m_queuedEvents.pop();
    }

    // process each delayed event: they won't be sent because of EventManager shutdown
    while (!m_delayedEvents.empty())
    {
        // retrieve and delete memory
        auto eventPair = m_delayedEvents.front();
        delete eventPair.first;
        m_delayedEvents.pop_front();
    }

    // clear list of event listeners
    m_eventListeners.clear();
}

// block and send event to all listeners for that type
// usage notes in header file
void EventManager::SendEvent(Event* ev)
{
    // retrieve ref to appropriate set of listeners based on event type
    auto& listenerList = m_eventListeners[static_cast<size_t>(ev->m_type)];

    // send the event to each listener in that list
    for (EventListener* listener : listenerList)
    {
        listener->OnEvent(ev);
    }
}

// queue event to be sent later, event dynamically allocated
// usage notes in header file
void EventManager::QueueEvent(Event* ev, float timeDelay)
{
    // case delay time is zero: queue to end of frame
    if (timeDelay == 0.f)
    {
        // add to list of queued events
        m_queuedEvents.push(ev);
    }
    // case delay is non-zero: queue for dispatch later after time passes
    else
    {
        // add to list of delayed events
        m_delayedEvents.emplace_back(ev, timeDelay);
    }
}

// to remove listener, unsubscribe all on that listener
void EventManager::RemoveListener(EventListener* listener)
{
    listener->UnsubscribeAll();
}

// internal subscribe call
// assumes listener is persistent and not temporary or local memory
void EventManager::Subscribe(EventListener* listener, int eventType)
{
    // insert listener into appropriate set in list of sets
    m_eventListeners[eventType].insert(listener);
}

// internal unsubscribe call
// assumes listener is persistent and not temporary or local memory
void EventManager::Unsubscribe(EventListener* listener, int eventType)
{
    // remove listener from appropriate set in list of sets
    m_eventListeners[eventType].erase(listener);
}

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
