/*********************************************************************************/
/*!
@file           Bar.h

@brief          xxx.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

*/
/*********************************************************************************/

#ifndef __BAR_H__
#define __BAR_H__

#include "MidiFile.h"

// The event bits can be ORed together
#define EVENT_BITS_playingStopped          0x0001 // set when we reach the end of piece
#define EVENT_BITS_forceFullRedraw         0x0002 // force the whole screen to be redrawn
#define EVENT_BITS_forceRatingRedraw       0x0004 // force the score to be redrawn
#define EVENT_BITS_newBarNumber            0x0008 // force the bar number to be redrawn
#define EVENT_BITS_UptoBarReached          0x0010 // Used for looping when playing between two bars.
#define EVENT_BITS_loadSong                0x0020 // load song

typedef unsigned long eventBits_t;

// controls the bar numbers
class CBar
{
public:

    CBar() { reset(); }

    // You MUST clear the time sig to 0 first before setting an new start Time Signature
    // at the start of the piece of music
    void setTimeSig(int top, int bottom);

    int getTimeSigTop() {return m_currentTimeSigTop;} // The Numerator
    int getBeatLength() {return m_beatLength;}
    int getBarLength() {return m_barLength;} // in ppqn

    // Sets the bar position from which playback should start. Only supports seeking forward.
    //
    // Parameters:
    // - bar: Target bar position (can include fractional positions)
    //        Must be >= current position for seeking to work correctly
    //
    // Side effects:
    // - Updates m_playFromBar to the target position
    // - Recalculates m_playUptoBar = m_playFromBar + m_loopingBars  
    // - Sets m_seekingBarNumber flag via checkGotoBar() when target is ahead
    // - Updates m_enablePlayFromBar and m_enableLooping flags
    //
    // Usage:
    // - Call with desired bar number to jump forward to that position
    // - Use fractional values for precise positioning within bars
    // - Commonly used for practice loops and section repetition
    // - To move backward, song must be restarted from beginning
    void setPlayFromBar(double bar);
    void setPlayFromBar(int bar, int beat = 0, int ticks = 0);
    void reset() {
        setTimeSig( 0 , 0);
        m_playFromBar = 0.0;
        m_playUptoBar = 0.0;
        m_loopingBars = 0.0;
        m_seekingBarNumber = false;
        m_flushTicks = false;
        m_eventBits = 0;
        setupEnableFlags();
    }

    void setPlayUptoBar(double bar);
    double getPlayUptoBar(){ return m_playUptoBar;}
    void setLoopingBars(double bars);
    double getLoopingBars(){ return m_loopingBars;}

    void rewind() {
        int top = m_startTimeSigTop;
        int bottom = m_startTimeSigBottom;

        setTimeSig( 0 , 0);
        setTimeSig(top, bottom);
        checkGotoBar();
    }
    void getTimeSig(int *top, int *bottom) {
        *top = m_currentTimeSigTop;
        *bottom = m_currentTimeSigBottom;
    }

    //return true if bar number has changed
    int addDeltaTime(int ticks);

    //
    int getBarNumber(){ return m_barCounter;}

    double getCurrentBarPos()
    {
        return m_barCounter + static_cast<double>(m_beatCounter)/m_currentTimeSigBottom +
            static_cast<double>(m_deltaTime)/(m_beatLength * m_currentTimeSigBottom * SPEED_ADJUST_FACTOR);
    }

    bool seekingBarNumber() { return m_seekingBarNumber;}

    // get and reset the current bar event bits
    eventBits_t readEventBits() {
        eventBits_t bits = m_eventBits;
        m_eventBits = 0;
        return bits;
    }

    int goToBarNumer();

private:
    void checkGotoBar();
    void setupEnableFlags()
    {
        m_enableLooping = (m_loopingBars > 0.0)?true:false;
        m_enablePlayFromBar = (m_enableLooping || m_playFromBar > 0.0)?true:false;
    }

    // This member variable tracks the accumulated time (in MIDI ticks) within the current beat 
    // of a bar. It represents the fractional part of the current beat position.
    //
    // Invariants:
    // - Should always be non-negative
    // - Should never exceed m_beatLength * SPEED_ADJUST_FACTOR
    // - Gets incremented by addDeltaTime() 
    // - Wraps back to 0 when exceeding m_beatLength, incrementing m_beatCounter
    //
    // Usage:
    // - Used to calculate precise positions within a beat for timing and display
    // - Critical for maintaining musical timing and bar/beat synchronization
    // - Contributes to getCurrentBarPos() calculations for fractional bar positions
    int m_deltaTime;
    int m_beatLength; //in ppqn ticks
    int m_barLength; // m_beatLength * getTimeSigTop() (also in ppqn ticks)

    int m_startTimeSigTop; // The time Sig at the start of the piece
    int m_startTimeSigBottom;
    int m_currentTimeSigTop; // The current time Sig at the start of the piece
    int m_currentTimeSigBottom;

    int m_barCounter;
    int m_beatCounter;

    // Stores the starting bar position for playback, supporting fractional bar positions
    // for precise control. Used in conjunction with m_playUptoBar for section looping.
    //
    // Invariants:
    // - Must be >= 0.0
    // - When changed, triggers update of m_playUptoBar = m_playFromBar + m_loopingBars
    // - Affects m_enablePlayFromBar state
    //
    // Usage:
    // - Set through setPlayFromBar() methods
    // - Used by checkGotoBar() to determine if seeking is needed
    // - Provides reference point for getCurrentBarPos() comparisons
    // - Key component in implementing practice sections and loops
    double m_playFromBar;
    double m_playUptoBar;
    double m_loopingBars;
    bool m_seekingBarNumber;
    bool m_flushTicks;

    // Bit field that tracks various bar-related events that need to be communicated 
    // to other parts of the system. Events include playback state changes, UI updates,
    // and bar position changes.
    //
    // Invariants:
    // - Can be any combination of EVENT_BITS_* flags OR'ed together
    // - Cleared after being read via readEventBits()
    // - Set by various bar operations like addDeltaTime() and checkGotoBar()
    //
    // Events:
    // - EVENT_BITS_playingStopped: End of piece reached
    // - EVENT_BITS_forceFullRedraw: Request complete screen redraw
    // - EVENT_BITS_forceRatingRedraw: Request score redraw
    // - EVENT_BITS_newBarNumber: Bar number changed
    // - EVENT_BITS_UptoBarReached: Loop end point reached
    // - EVENT_BITS_loadSong: Song needs to be loaded
    //
    // Usage:
    // - Used for inter-component communication about bar state changes
    // - Drives UI updates and playback control decisions
    // - Read and cleared atomically through readEventBits()
    eventBits_t m_eventBits;

    // Flag that indicates whether bar looping is active. When enabled, playback will 
    // repeat between m_playFromBar and m_playUptoBar positions.
    //
    // Invariants:
    // - True if m_loopingBars > 0.0
    // - False otherwise
    // - Only modified by setupEnableFlags()
    //
    // Usage:
    // - Controls whether EVENT_BITS_UptoBarReached is set in checkGotoBar()
    // - Used to implement repeating sections of music during practice
    // - Works in conjunction with m_playFromBar and m_playUptoBar to define loop boundaries
    bool m_enableLooping;

    // Flag that controls whether playback should start from a specific bar position.
    // Gets set when either looping is enabled or when a specific start bar is set 
    // via setPlayFromBar().
    //
    // Invariants:
    // - True if m_loopingBars > 0.0 or m_playFromBar > 0.0
    // - False otherwise
    // - Only modified by setupEnableFlags()
    //
    // Usage:
    // - Controls whether checkGotoBar() is called during addDeltaTime()
    // - Used to optimize bar position checking when no specific start point is needed
    bool m_enablePlayFromBar;

};

#endif  // __BAR_H__

