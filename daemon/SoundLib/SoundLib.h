/**
 * @file
 * The library for control of the sound system volume
 *
 **
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Daniel Haimov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SOUND_VOL_CONTROL_H_
#define SOUND_VOL_CONTROL_H_

#include <stdbool.h>

/**
 * Make the sound state muted
 */
void mute();

/**
 * Unmute the sound state
 */
void unmute();

/**
 * Change the current value by the given value
 * if the given value > 0, then increase the current volume by value
 * if the given value < 0, then decrease the current volume by value
 * @param value The value of change. 
 */
void chgVol(const int value);

/**
 * Get current volume
 * @return The current volume percents
 */
const long getVol();

/**
 * Is the sound state muted
 * @return True if the sound state is muted
 */
const bool isMuted();

#endif
