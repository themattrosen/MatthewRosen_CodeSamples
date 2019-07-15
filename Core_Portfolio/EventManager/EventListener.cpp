/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   EventListener.cpp - v1.0
	Author: Matthew Rosen

	Summary:
		Implementation of EventListener functionality. Includes ability to subscribe
        to different EventTypes.

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// includes
#include "EventListener.h"
#include "EventManager.h"

// destructor calls Unsubscribe all to ensure EventManager doesn't hold on to garbage data.
EventListener::~EventListener()
{
    UnsubscribeAll();
}

// subscribe this to an EventType in the EventManager
void EventListener::Subscribe(EventType type)
{
    // find the type in the subscribe list to avoid unnecessary add
    auto iter = m_subscribeList.find(type);
    if (iter == m_subscribeList.end())
    {
        // call EventManager subscribe with the this ptr.
        EventManager::Instance().Subscribe(this, static_cast<int>(type));

        // add new type to subscribe list
        m_subscribeList.insert(type);
    }
}

// unsubscribe this from an EventType in the EventManager
void EventListener::Unsubscribe(EventType type)
{
    // find the type in the subscribe list: don't unsubscribe if not subscribed
    auto iter = m_subscribeList.find(type);
    if (iter != m_subscribeList.end())
    {
        // call EventManager unsusbscribe with the this ptr
        EventManager::Instance().Unsubscribe(this, static_cast<int>(type));

        // remove type from the subscribe list
        m_subscribeList.erase(type);
    }
}

// unsubscribe from all types
void EventListener::UnsubscribeAll()
{
    // range based loop through each type in the subscribe list
    for (EventType type : m_subscribeList)
    {
        // call EventManager unsubscribe with the this ptr
        EventManager::Instance().Unsubscribe(this, static_cast<int>(type));
    }

    // clear the subscribe list
    m_subscribeList.clear();
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
