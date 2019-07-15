# MatthewRosen_CodeSamples
Here lies code written by Matthew Rosen in projects both personal and academic.
The code is divided into subsections based on job family that it relates to.

## Tools
### BehaviorTree Node Editor
This was a side project turned into a contribution to my junior Game project (Haphazard300 engine for the game Cat's Cradle). 
Written using ImGui, this tool was originally a node editor for creating DSP effects from a node based DSP graph for a personal project.
It was easily adapted into an editor for BehaviorTrees for the AI in the engine. 
Although the game never ended up needing AI, the editor and the BehaviorTrees are still fully functional.
The node editor backend was written by me, and the actual AI part of the BehaviorTrees was implemented by Brett Schiff. 
The code provided is not buildable, but there is a video demo of the product in the repo and at: <https://youtu.be/frY-5A-Dphs>.

### Next Release
Reverb filter tool.

## Audio/DSP
### Plucked String Filter Music

This was an extra credit class assignment for Mathematics of Digital Signal Processing I (MAT320) at DigiPen Institute of Technology. 
The assignment was to use the plucked string filter from the previous assignment and 
use it to output a .wav file that contained at least 8 bars of a song with a minimum of 2 voices playing at once somewhere in the song. 
  
The final product was the ability to define a song in a separate file as a series of notes within measures, and it will output a .wav file with that song played using a plucked string filter.
  
#### The Plucked String Filter

The plucked string filter is implemented using a comb filter, basic lowpass filter, and an allpass filter in series feeding back into the comb filter as described in Stieglitz's book A Digital Signal Processing Primer.

### Next release
Reverb Filter
  
## Core/Engine
### Haphazard Event System
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
Internally, macros would automagically preprocess this into an enum, a global array of strings, and a struct inheriting from Event with your data. 
Because this was done with the preprocessor, a programmer could add an event to the registry file, and then use it with intellisense in Visual Studio immediately without any extra work.
  
It was incredibly easy to use and extend in a project of the scope that it was used for: a student project. 
Adding new event types was easy but required a lengthy recompile of everything that included an event system file.
Because of this, this style of architecture would not scale well for a larger project unless a comprehensive list of all event types were 
defined at the beginning of the project and none were added during development. A better implementation would be to make it data driven so designers could add event types on the fly in scripts and in the editor without recompiling the entire engine. 

### Next release
Haphazard300 Component Storage System
