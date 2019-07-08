/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   plucked_music.cpp - v1.0
	Author: Matthew Rosen

	Summary:
		Defines AudioData output, Songs/Measures/Notes, and playing songs to 
		output and writing it to a .wav file.

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// includes
#include "filters.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

// defines for convenience
#define stream std::cout
#define endl std::endl

// helper macros
#define SHORT_TO_FLOAT(s) (static_cast<float>(s) / static_cast<float>((1 << (16 - 1))))
#define FLOAT_TO_SHORT(s) (static_cast<short>(s * static_cast<float>((1 << (16 - 1)) - 1)))

// constants
const float PI_F = 3.141593f;					// global value of PI
const unsigned RATE = 44100;					// global sampling rate used for .wav file output
const unsigned BPM = 104;						// constant BPM of the song
const float QUARTER_NOTE = 0.28818443f;			// number of seconds that a quarter note lasts at BPM
const float HALF_NOTE = QUARTER_NOTE * 2.f;		// number of seconds for a half note
const float EIGHTH_NOTE = QUARTER_NOTE / 2.f;	// number of seconds for an eighth note
const float SUS_NOTE = 0.9999999f;				// contant used to make the plucked string filter sustain for longer

// AudioData holds raw audio samples
// Hardcoded to use 16 bit samples and 44.1 kHz output
struct AudioData
{
	std::vector<float> data;

	AudioData() : data(0) {}
	float rate() const { return static_cast<float>(RATE); }
	unsigned size_in_bytes() const { return static_cast<unsigned>(data.size()) * sizeof(short); }
	unsigned num_samples() const { return static_cast<unsigned>(data.size()); }
	short bits_per_sample() const { return 16; }
};

// helper function to write the header of a wave file to a file
static void write_header(std::fstream& output, unsigned sizeInBytes)
{
	// define an anonymous struct to encompass the data
	struct {
		char riff_chunk[4];
		unsigned chunk_size;
		char wave_fmt[4];
		char fmt_chunk[4];
		unsigned fmt_chunk_size;
		unsigned short audio_format;
		unsigned short number_of_channels;
		unsigned sampling_rate;
		unsigned bytes_per_second;
		unsigned short block_align;
		unsigned short bits_per_sample;
		char data_chunk[4];
		unsigned data_chunk_size;
	}
	header = { {'R','I','F','F'},
			   36 + sizeInBytes,
			   {'W','A','V','E'},
			   {'f','m','t',' '},
			   16,1,1,RATE,sizeof(short) * RATE,2,16,
			   {'d','a','t','a'},
			   sizeInBytes
	};

	// write the header as an array of bytes
	output.write(reinterpret_cast<char*>(&header), 44);
}

// normalizes an audio data output
static void normalize(AudioData& data)
{
	float maxVal = 0.f;
	const float dB = -1.5f;
	for (unsigned i = 0; i < data.num_samples(); ++i)
	{
		if (std::abs(data.data[i]) > maxVal)
			maxVal = std::abs(data.data[i]);
	}

	const float target = std::pow(10.0f, dB / 20.0f);
	float gain = target / maxVal;

	for (unsigned i = 0; i < data.num_samples(); ++i)
	{
		data.data[i] *= gain;
	}
}

// write audio data out to a wave file
static void write_wave(const char* filename, const AudioData& data)
{
	// open the file for binary write
	std::fstream out(filename, std::ios_base::binary | std::ios_base::out);

	// allocate a new array of 16 bit integers for the samples
	short* outData = new short[data.num_samples()];

	// convert all the AudioData floating pt samples to 16 bit int samples
	for (unsigned i = 0; i < data.num_samples(); ++i)
	{
		float nextF = data.data[i];
		short nextS = FLOAT_TO_SHORT(nextF);
		outData[i] = nextS;
	}

	// write the header information of the wave file
	write_header(out, data.size_in_bytes());

	// write the wave file data chunk
	out.write(reinterpret_cast<char*>(outData), data.size_in_bytes());

	// delete allocated memory
	delete[] outData;
}

// helper function to convert a given note to a frequency
// @param noteName: a string with either 1 or 2 characters. "<Note letter><modifier>", e.g. Eb for "E flat"
// @param octave:   integer octave number for the note to be in. Values should range from -1 to 7.
// @return the given note's frequency in Hz
static float note_to_frequency(const std::string& noteName, int octave)
{
	const float baseFreq = 440.f;
	float modifier = 0;
	char noteChar = 0;

	// extract a modifier if it exists (modifier is 1 if note is sharp or -1 if note is flat, 0 otherwise)
	if (noteName.size() > 1)
	{
		char modChar = noteName[1];
		if (modChar == 'b')
			modifier = -1.f;
		else if (modChar == 's' || modChar == '#')
			modifier = 1.f;
	}

	// extract the note letter
	if (noteName.size() >= 1)
	{
		noteChar = noteName.front();
	}

	// verify note letter and octave are valid
	if (noteChar < 'A' || noteChar > 'G' || octave < -1 || octave > 7)
		return -1.f; // error with the note

	float noteVal = 0.f; // noteVal is the half-step increment from A

	// determine the half-step increment from A
	switch (noteChar)
	{
	case 'A':
		noteVal = 0;
		break;
	case 'B':
		noteVal = 2.f; // half steps away from A
		break;
	case 'C':
		noteVal = 3.f; // half steps away from A
		break;
	case 'D':
		noteVal = 5.f; // half steps away from A
		break;
	case 'E':
		noteVal = 7.f; // half steps away from A
		break;
	case 'F':
		noteVal = 8.f; // half steps away from A
		break;
	case 'G':
		noteVal = 10.f; // half steps away from A
		break;
	// default case guaranteed to never be hit
	}

	// calculate the frequency without octave modification using the sharp/flat modifier from above
	float freq = baseFreq * std::pow(2.f, (noteVal + modifier) / 12.f);

	// modify octave value based on if note is below a C (A and B are in the octave below)
	// modification is because base frequency is in octave 3
	if (noteVal < 3.f) octave -= 4;
	else octave -= 5;

	// convert to frequency multiplier
	float octaveMult = std::pow(2.f, octave);

	return freq * octaveMult;
}

// ease of use with defining a song
#define N(n, o) note_to_frequency(#n, o)
#define NOTE(note, octave, ...) Note(N(note, octave), __VA_ARGS__)
#define MEASURE(...) {{}, { __VA_ARGS__ }}

// note within a song definition
struct Note
{
	float beatDuration;	// number of beats to sustain for
	PSF filter;			// plucked string filter to sample from
	float currentTime;	// current time tracker
	float barOffset;	// beat offset in the measure

	// ctors
	Note(float freq, float duration, float _barOffset, float RVal = 0.99985f) 
		: beatDuration(duration), filter(freq, duration, RVal), currentTime(0.f), barOffset(_barOffset) {}
	Note& operator=(const Note&) = default;
};

// measure within a song definition
struct Measure
{
	std::vector<Note> sustainedNotes;	// notes sustained from the previous measure and during play
	std::vector<Note> notesToAdd;		// notes that will play this measure
};

// song definition: nothing more than a name and a list of measures
struct Song
{
	std::string name;				// wav filename
	std::vector<Measure> measures;	// list of measures that make the song
};

// helper function to play a measure to output AudioData
static void play_measure(AudioData& data, Measure& measure)
{
	const float length_seconds = 4.f * QUARTER_NOTE;	// length of the measure in seconds
	const float sample_length = 1.f / RATE;				// length of 1 sample in seconds
	const float length_samples = RATE * length_seconds;	// number of samples in the measure

	// for each sample in the measure
	for (float i = 0.f; i < length_seconds; i += sample_length)
	{
		// average together each note playing simultaneously to get the final sample output for this sample
		float average = 0.f;
		unsigned numSamples = 0; // number of notes playing during this sample

		// sample sustained notes
		for (Note& note : measure.sustainedNotes)
		{
			average += note.filter();
			++numSamples;
			note.currentTime += sample_length;
		}

		// add new notes if applicable
		for (size_t j = 0; j < measure.notesToAdd.size(); ++j)
		{
			Note& note = measure.notesToAdd[j];

			// case add the new note
			if (note.barOffset * QUARTER_NOTE <= i)
			{
				// add to list of sustained notes
				measure.sustainedNotes.push_back(note);

				// delete from list of notes to add
				measure.notesToAdd.erase(measure.notesToAdd.begin() + j);
				--j;

				// sample next note
				average += measure.sustainedNotes.back().filter();
				++numSamples;
				measure.sustainedNotes.back().currentTime += sample_length;
			}
		}
		
		// removed notes that are finished
		for (size_t j = 0; j < measure.sustainedNotes.size(); ++j)
		{
			Note& note = measure.sustainedNotes[j];
			if (note.currentTime >= note.beatDuration * QUARTER_NOTE)
			{
				measure.sustainedNotes.erase(measure.sustainedNotes.begin() + j);
				--j;
			}
		}

		// add next sample to the output data
		data.data.push_back(average / static_cast<float>(numSamples));
	}
}

// function to play a song to an AudioData output
static void play_song(AudioData& data, Song& song)
{
	// length of the song in seconds
	float length_seconds = static_cast<float>(song.measures.size()) * 4.f * QUARTER_NOTE;
	float length_samples = RATE * length_seconds;	// length of the song in samples
	std::vector<Note> sus;							// notes carried over from previous measures

	// process each measure in the song
	for (Measure& measure : song.measures)
	{
		// add sustained notes from the previous measure to the current measure
		measure.sustainedNotes.clear();
		for (Note note : sus)
			measure.sustainedNotes.push_back(note);

		// play the current measure
		play_measure(data, measure);

		// add notes sustained from the measure to the next measure
		sus.clear();
		for(Note note : measure.sustainedNotes)
			sus.push_back(note);
	}
}

// main: plays 
int main(void)
{
	// took the first 40 measures of the song Mister Sandman from this musescore score.
	//https://musescore.com/user/1187206/scores/968751

	// defined the song in a separate file
	// to make your own song, define it in another file and include that file here using the form:
	// {
	//     "songname.wav",
	//     {
	//         MEASURE( NOTE(Ab, 3, 2, 0, SUS_NOTE), NOTE(<note name>, <octave>, <note duration in beats>, <bar offset in beats>, <optional: use SUS_NOTE to sustain the note for longer than 1 beat>) ),
	//         ... more measures here
	//         MEASURE( )
	//     }
	// };
	Song song =
#		include "Song.songdef"

	// process the song (costly operation)
	AudioData data;

	// play song to data file
	play_song(data, song);

	normalize(data);

	// write the data to a file
	write_wave(song.name.c_str(), data);

	return 0;
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
