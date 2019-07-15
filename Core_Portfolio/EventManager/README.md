## EventManager

This was the event system used in the Haphazard 300 game engine. I've written an article on LinkedIn on its implementation here: 
  
https://www.linkedin.com/pulse/students-take-implementing-event-systems-games-using-matthew-rosen/
  
The EventManager was based off of the Observer pattern in Object-Oriented Programming. Persistent objects could
inherit from EventListener and subscribe to events that it wanted to listen to. When gameplay or internal systems sent out those events,
only the listeners subscribed to that type of event would receive it to process as they wished. Events are defined as 
c-style POD structs inheriting from a base Event struct. This way subclasses of Event can parcel data to be sent to listeners as necessary.
Events were defined in a registry file that was easily added to. In order to add an event you simply had to add a line that had your
event name and whatever data you wanted to add inside of it, like so:
```
EVENT(MyEvent, float timeSent, int numTimesSent, void* ptrToData)
```
Internally, macros would automatically preprocess this into an enum, a global array of strings, and a struct inheriting from Event with your data. 
Because this was done with the preprocessor, a programmer could add an event to the registry file, and then use it with intellisense in Visual Studio immediately without any extra work.
  
It was incredibly easy to use and extend in a project of the scope that it was used for: a student project. 
Adding new event types was easy but required a lengthy recompile of everything that included an event system file.
Because of this, this style of architecture would not scale well for a larger project unless a comprehensive list of all event types were 
defined at the beginning of the project and none were added during development. A better implementation would be to make it data driven so designers could add event types on the fly in scripts and in the editor without recompiling the entire engine. 
