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

#include "SoundLib.h"

#include <alsa/asoundlib.h>
#include <stdlib.h>

#include "Log.h"

#define TAG "SOUND_LIB"         /**< The tag for log file records */

#define AUDIO_CARD  "default"   /**< The name of the default sound card */
#define AUDIO_MIXER "Master"    /**< The name of the used mixer */

#define ERR 1                   /**< Error code */
#define NO_ERR 0                /**< No error code */

#define MAX_VOL 100             /**< The maximal volume in percents */

/**
 * \enum AUDIO_ACTIONS
 * The actions for audio
 */
enum AUDIO_ACTIONS
    {
	AUDIO_VOLUME_SET_VOLUME,
	AUDIO_VOLUME_GET_VOLUME,
	AUDIO_VOLUME_SET_MUTE,
	AUDIO_VOLUME_GET_MUTE
    };

/**
 * \enum SOUND_STATE
 * The states of sound: muted, unmuted
 */
enum SOUND_STATE { UNMUTED, MUTED };

long volume_, state_;   /**< Current volume percent and the state of sound */
long minv, maxv;        /**< Minimal and maximal volume */

snd_mixer_t* handle;
snd_mixer_elem_t* elem;

/**
 * Make the given action on the sound volume, e.g. mute or get the current
 * volume value.
 * @param action The action type: get mute state or set mute, get or set volume
 * @param vol The current sound volume
 * @return ERR or NO_ERR
 */
const int doSoundVolAction(const int action, long *vol)
{
    int mute;
    int res;
    switch (action)
	{
	  case AUDIO_VOLUME_GET_VOLUME:
	      res = snd_mixer_selem_get_playback_volume(elem, 0, vol);
	      if(res != NO_ERR)
		  {
		      writeToLog2("ERR: Can't get volume ", snd_strerror(res), TAG);
		      return ERR;
		  }
	      *vol = MAX_VOL * (*vol) / (maxv - minv);
	      break;
	      
	  case AUDIO_VOLUME_SET_VOLUME:
	      *vol = (*vol * (maxv - minv) / (MAX_VOL - 1)) + minv;
	      res = snd_mixer_selem_set_playback_volume_all(elem, *vol);
	      if(res != NO_ERR)
		  {
		      writeToLog2("ERR: Can't set volume ", snd_strerror(res), TAG);
		      return ERR;
		  }
	      *vol = MAX_VOL * (*vol) / (maxv - minv);
	      break;
	      
	  case AUDIO_VOLUME_GET_MUTE:
	      res = snd_mixer_selem_get_playback_switch(elem, 0, &mute);
	      if(res != NO_ERR)
		  {
		      writeToLog2("ERR: Can't get mute state ", snd_strerror(res), TAG);
		      return ERR;
		  }
	      *vol = mute ? UNMUTED: MUTED;
	      break;
	      
	  case AUDIO_VOLUME_SET_MUTE:
	      mute = (*vol == 0) ? MUTED : UNMUTED;
	      res = snd_mixer_selem_set_playback_switch_all(elem, mute);
	      if(res != NO_ERR)
		  {
		      writeToLog2("ERR: Can't set mute state ", snd_strerror(res), TAG);
		      return ERR;
		  }
	      break;
	}

    return NO_ERR;
}

/**
 * Initialise the sound control
 * @return The sound control handler
 */
const int initSoundControl()
{
    snd_mixer_selem_id_t* sid;    
        
    snd_mixer_selem_id_malloc(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, AUDIO_MIXER);

    int res = snd_mixer_open(&handle, 0);
    if(res < 0)
	{
	    writeToLog2("ERR: Can't open mixer ", snd_strerror(res), TAG);
	    return ERR;
	}
    res = snd_mixer_attach(handle, AUDIO_CARD);
    if(res < 0)
	{
	    writeToLog2("ERR: Can't attach the open mixer ", snd_strerror(res), TAG);
	    return ERR;
	}
    res = snd_mixer_selem_register(handle, NULL, NULL);
    if(res < 0)
	{
	    writeToLog2("ERR: Can't register the open mixer ", snd_strerror(res), TAG);
	    return ERR;
	}
    res = snd_mixer_load(handle);
    if(res < 0)
	{
	    writeToLog2("ERR: Can't load the open mixer ", snd_strerror(res), TAG);
	    return ERR;
	}

    //    elem = snd_mixer_first_elem(handle);
    //    snd_mixer_selem_get_id(elem, sid);
    //    writeToLog2("Name: ", snd_mixer_selem_id_get_name(sid), TAG);
    elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_id_free(sid);
    
    res = snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);
    if(res < 0)
	{
	    writeToLog2("ERR: Can't get the playback volume of the open mixer ", snd_strerror(res), TAG);
	    return ERR;
	}
    
    res = doSoundVolAction(AUDIO_VOLUME_GET_VOLUME, &volume_);
    if(res == NO_ERR)
	res = doSoundVolAction(AUDIO_VOLUME_GET_MUTE, &state_);
    return res;
}

/**
 * Finish sound control using
 */
void finishSoundControl()
{
    int res = snd_mixer_close(handle);
    if(res != NO_ERR)
	writeToLog2("ERR: Can't close the open mixer ", snd_strerror(res), TAG);
    
    res = snd_config_update_free_global();
    if(res != NO_ERR)
	writeToLog2("ERR: Can't free the sound resources ", snd_strerror(res), TAG);
}

/**
 * Make the sound state muted
 */
void mute()
{
    const int res = initSoundControl();
    
    if((res == NO_ERR) && (state_ == UNMUTED))
	{
	    state_ = MUTED;
	    doSoundVolAction(AUDIO_VOLUME_SET_MUTE, &state_);
	}

    finishSoundControl();
}

/**
 * Unmute the sound state
 */
void unmute()
{
    const int res = initSoundControl();
	
    if( (res == NO_ERR) && (state_ == MUTED) )
	{
	    state_ = UNMUTED;
	    doSoundVolAction(AUDIO_VOLUME_SET_MUTE, &state_);
	}

    finishSoundControl();
}

/**
 * Change the current value by the given value
 * if the given value > 0, then increase the current volume by value
 * if the given value < 0, then decrease the current volume by value
 * @param value The value of change. 
 */
void chgVol(const int value)
{
    const int res = initSoundControl();
    
    if(res == NO_ERR)
	{
	    if(value > 0)
		volume_ = (volume_ + value) <= MAX_VOL ? volume_ + value: MAX_VOL;
	    else
		volume_ = (volume_ + value) >= 0 ? volume_ + value : 0;
	    doSoundVolAction(AUDIO_VOLUME_SET_VOLUME, &volume_);
	}
    
    finishSoundControl();
}

/**
 * Get current volume
 * @return The current volume percents
 */
const long getVol()
{
    initSoundControl();
    finishSoundControl();
    return volume_;
}

/**
 * Is the sound state muted
 * @return True if the sound state is muted
 */
const bool isMuted()
{
    const int res = initSoundControl();
    
    if(res == NO_ERR)
	doSoundVolAction(AUDIO_VOLUME_GET_MUTE, &state_);
    
    finishSoundControl();
    
    return state_ == MUTED;
}
