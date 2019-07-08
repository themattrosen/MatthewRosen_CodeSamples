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
### Next release
Haphazard300 Event System
