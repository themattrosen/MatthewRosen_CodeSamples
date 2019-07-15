/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   Util.h - v1.0
	Author: Matthew Rosen

	Summary:
		Macro helper file, with solutions pulled from users on the internet.
        Includes getting the number of arguments in a __VA_ARGS__ list, and
        applying a given macro on each argument listed in __VA_ARGS__.

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// helper macros for microsoft compiler specific reasons
// microsoft compiler has to be coerced into doing these things, gcc does these just fine
#define EXPAND(x) x 
#define PASTE(a, b) a ## b
#define XPASTE(a, b) PASTE(a, b)

// find the number of args in __VA_ARGS__ at compile time
//https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
// user mchiasson
#define _NUM_VA_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9, _10, _11, _12, _13, _14, _15, _16, _17, N, ...) N 
#define NUM_VA_ARGS(...) EXPAND(_NUM_VA_ARGS(__VA_ARGS__, 17, 16, 15, 14, 13, 12, 11, 10, 9,8,7,6,5,4,3,2,1,0, -1))

// Call macro for each argument
//https://stackoverflow.com/questions/6707148/foreach-macro-on-macros-arguments
// user luser droog
#define APPLY_0(M) 
#define APPLY_1(M, a) M(a)
#define APPLY_2(M, a, b) M(a) M(b)
#define APPLY_3(M, a, b, c) M(a) M(b) M(c)
#define APPLY_4(M, a, b, c, d) M(a) M(b) M(c) M(d)
#define APPLY_5(M, a, b, c, d, e) M(a) M(b) M(c) M(d) M(e)
#define APPLY_6(M, a, b, c, d, e, f) M(a) M(b) M(c) M(d) M(e) M(f)
#define APPLY_7(M, a, b, c, d, e, f, g) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g)
#define APPLY_8(M, a, b, c, d, e, f, g, h)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h)
#define APPLY_9(M, a, b, c, d, e, f, g, h, i)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i)
#define APPLY_10(M, a, b, c, d, e, f, g, h, i, j)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j)
#define APPLY_11(M, a, b, c, d, e, f, g, h, i, j, k)    \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k)
#define APPLY_12(M, a, b, c, d, e, f, g, h, i, j, k, l) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l)
#define APPLY_13(M, a, b, c, d, e, f, g, h, i, j, k, l, m)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m)
#define APPLY_14(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n)
#define APPLY_15(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)    \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o)
#define APPLY_16(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o) M(p)
#define APPLY_17(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o) M(p) M(q)
#define APPLYX(APP, M, ...) EXPAND(APP(M, __VA_ARGS__))

#define APPLY_ON_EACH(M, ...) APPLYX(EXPAND(XPASTE(APPLY_, NUM_VA_ARGS(__VA_ARGS__))), M, __VA_ARGS__)

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
