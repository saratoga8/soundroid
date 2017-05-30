/**
 * @file 
 * The connector to the sound system
 * Control the sound system's volume and state(mute/unmute)
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

#include "SndConnector.h"
#include "CommandsNames.h"

extern "C" {
	#include "SoundLib.h"
	#include "Log.h"
}

using namespace std;

const char* SndConnector::TAG = "SND_CONNECTOR";                   /**< The tag for writting to log file */

/**
 * Set the given string as arrived data
 * @param str The string for setting of the arrived data
 */
void SndConnector::setArrivedDataStr(const string &str)
{
    mutex_.lock();
    dataStr_ = str;
    mutex_.unlock();
}

/**
 * Receive data from the connector
 * @return The data string 
 */
const string SndConnector::receive()
{
    string str(dataStr_);
    dataStr_ = "";
    return str;
}

/**
 * Mute the sound system volume
 */
void SndConnector::doMute()
{
    writeToLog(string("Execute mute\n").c_str(), TAG);
    mute();
    setArrivedDataStr(OK);
}

/**
 * Unmute the sound system volume
 */
void SndConnector::doUnmute()
{
    writeToLog(string("Execute unmute\n").c_str(), TAG);
    unmute();
    setArrivedDataStr(OK);
}

/**
 * Change the sound system volume to the given value
 * @param value The value in percent
 */
void SndConnector::doChgVol(const int value)
{
    writeToLog(string("Change volume by value " + to_string(value) + "\n").c_str(), TAG);
    chgVol(value);
    setArrivedDataStr(OK);
}

/**
 * Get the current value (in percent) of the sound system volume
 * @return The string of the current volume value
 */
const string SndConnector::doGetVol()
{
    writeToLog(string("Get current volume\n").c_str(), TAG);
    const long vol = getVol();
    return to_string(vol);
}

/**
 * Is the current sound system state MUTE?
 * @return "true" of "false" strings
 */
const string SndConnector::doIsMuted()
{
    writeToLog(string("Check is muted?\n").c_str(), TAG);
    return (isMuted()) ? TRUE_: FALSE_;
}

