## Plucked String Filter Music

This was an extra credit class assignment for Mathematics of Digital Signal Processing I (MAT320) at DigiPen Institute of Technology. 
The assignment was to use the plucked string filter from the previous assignment and 
use it to output a .wav file that contained at least 8 bars of a song with a minimum of 2 voices playing at once somewhere in the song. 
  
The final product was the ability to define a song in a separate file as a series of notes within measures, and it will output a .wav file with that song played using a plucked string filter.
  
### The Plucked String Filter

The plucked string filter is implemented using a comb filter, basic lowpass filter, and an allpass filter in series feeding back into the comb filter as described in Stieglitz's book A Digital Signal Processing Primer. 

### Make your own songs

If you want to try out the program for your own song, you can simply modify the Song.songdef file.
  
Songs are defined as: 
```
{
    "MySongName.wav"
    {
        MEASURE( NOTE(Ab, 3, 2, 0, SUS_NOTE), NOTE(<note name, can have flat or sharp modifiers (b or #)>, <octave number>, <note duration in beats>, <bar offset in beats>, <optional: use SUS_NOTE to sustain the note for longer than 1 beat>) ),
	    //... more measures here
	    MEASURE( )
    }
};
```
  
When you've finished your song, you must recompile and run the program.
To compile with GCC, use the command:

`g++ -o plucked_music plucked_music.cpp -std=c++11`

Have fun with it!