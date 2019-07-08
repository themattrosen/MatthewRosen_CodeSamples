/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   filters.h - v1.0
	Author: Matthew Rosen

	Summary:
		Implementation of DSP filters used to generate plucked string tones.

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#ifndef __MAT320_FILTERS_H
#define __MAT320_FILTERS_H

// includes
#include <cmath>
#include <cstdlib>
#include <queue>

// helper macros
#define RAND_BETWEEN(min, max) (std::rand() % (max - min + 1) + min)
#define SHORT_TO_FLOAT(s) (static_cast<float>(s) / static_cast<float>((1 << (16 - 1))))
#define FLOAT_TO_SHORT(s) (static_cast<short>(s * static_cast<float>((1 << (16 - 1)) - 1)))

// global constants
extern const float PI_F;
extern const unsigned RATE;

// low pass filter
struct LPF
{
	// lowpass filter implements filter equation: 
	// y(t) = 0.5 * (x(t) + x(t - 1))
	
	float multVal;	// lowpass coefficient
	float x1;		// delayed input sample

	// default ctor
	LPF() : multVal(0.5f), x1(0.f) {}
	LPF& operator=(const LPF&) = default;

	// sample operator implements recurrence relation
	// inline to avoid instruction cache miss
	inline float operator()(float in)
	{
		float output = multVal * in + multVal * x1;
		x1 = in;
		return output;
	}
};

// all pass filter
struct APF
{
	// allpass filter implements filter equation:
	// y(t) = a * x(t) + x(t - 1) - a * y(t - 1)

	float a;	// allpass coefficient
	float x1;	// delayed input sample
	float y1;	// delayed output sample

	// ctor based off of a target frequency
	APF(float d, float freq) : a(0.f), x1(0.f), y1(0.f)
	{
		// calculate frequency in rad
		float w = PI_F * freq / RATE;
		a = (std::sin((1.f - d) * (w))) / (std::sin((1.f + d) * (w)));
	}

	APF& operator=(const APF&) = default;

	// sample operator implements recurrence relation
	// inline to avoid instruction cache miss
	inline float operator()(float next)
	{
		float output = a * next + x1 - a * y1;
		x1 = next;
		y1 = output;

		return output;
	}
};

// comb filter
struct CF
{
	// comb filter implements filter equation:
	// y(t) = x(t) + R^L * y(t - L)

	float R;					// distance from unit circle
	unsigned L;					// power of the comb
	std::queue<float> buffer;	// delayed output samples
	float multVal;				// R^L


	explicit CF(unsigned power, float RVal = 0.99985f) : R(RVal), L(power), buffer(), multVal(std::pow(R, L))
	{
		for (unsigned i = 0; i < L; ++i)
		{
			buffer.push(0.f);
		}
	}

	CF& operator=(const CF&) = default;
	
	// sample operator implements recurrence relation
	// inline to avoid instruction cache miss
	inline float operator()(float next)
	{
		float out = next + multVal * buffer.front();
		buffer.pop();

		return out;
	}

	// adds to the feedback
	inline void feed_back(float out)
	{
		buffer.push(out);
	}
};

// plucked string filter
// in this project designed to only play one note and sustain it.
struct PSF
{
	// plucked string filter implements filter diagram:
	//			_____________     ________________     ________________
	// x(t) -->| comb filter |-->| lowpass filter |-->| allpass filter |-----> y(t)
	//          -------------     ----------------     ----------------   |
	//               ^                                                    |
	//               |____________________________________________________|

private:
	float D;			// allpass coefficient calculated from sampling rate and target freuqncy
	LPF lowpass;		// lowpass filter
	APF allpass;		// allpass filter
	CF comb;			// comb filter
	float sus;			// sustain duration
	unsigned numSample;	// current sample index

public:
	float frequency;

	PSF(float freq, float duration = 1.f, float RVal = 0.99985f) : D(static_cast<float>(RATE) / freq - 0.5f), lowpass(),
		allpass(D - std::floor(D), freq),
		comb(std::floor(D), RVal), sus(duration), numSample(0), frequency(freq)
	{

	}

	PSF& operator=(const PSF&) = default;

	// sample operator implements recurrence relation
	// doesn't take input, generates input itself
	// inline to avoid instruction cache miss
	inline float operator()()
	{
		float next = 0.f;

		// case sample is within sustain duration
		if (numSample++ < 100 * static_cast<unsigned>(sus))
		{
			// generate random input
			short rangeBegin, rangeEnd, shortVal;
			rangeBegin = -15000;
			rangeEnd = 15000;
			shortVal = RAND_BETWEEN(rangeBegin, rangeEnd);
			next = SHORT_TO_FLOAT(shortVal);
		}
		// else case, next input is zero

		// process input through filters
		float combOut = comb(next);
		float lowOut = lowpass(combOut);
		float allOut = allpass(lowOut);

		// feedback to comb filter
		comb.feed_back(allOut);

		return allOut;
	}
};

#endif //__MAT320_FILTERS_H

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
