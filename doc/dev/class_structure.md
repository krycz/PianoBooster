# Core Classes
## CSong
* Represents a song loaded from a MIDI file.
* Inherits from CConductor to manage music playback and user interaction.
* Loads song data, handles MIDI events, and manages song state.
* Collaborates with CScore to display the musical score.

## CConductor
* The realtime MIDI engine, handling MIDI playback and input.
* Manages MIDI events, tempo, and user input from the MIDI keyboard.
* Collaborates with CScore to display the musical score.
* Incorporates features like transposing, boosting volume, and setting the skill level.

## CScore
* Manages the graphical representation of the musical score.
* Handles drawing of staves, notes, and other musical symbols.
* Collaborates with CSong and CConductor to display the musical score.

## CNotation
* This class is responsible for converting MIDI events into musical notation.
* It analyzes MIDI data and generates a sequence of musical symbols (notes, rests, and other musical symbols) that can be displayed on the sheet music.

## CMidiFile
* This class is responsible for reading and interpreting MIDI files.
* It parses the MIDI file format and extracts MIDI events, tempo information, time signatures, and other musical data.

## CScroll
* This class manages the scrolling of the musical score during playback.
* It handles the timing and synchronization of the display, ensuring that the correct portion of the score is shown as the music plays.

## CBar
* This class keeps track of the current bar and beat position in the musical score.
* It handles time signatures, bar markers, and beat markers.
* It also provides methods for setting the starting and looping points for playback.

## CMerge
* This class is responsible for merging multiple MIDI streams into a single stream.
* It is used to combine MIDI events from different tracks into a single timeline.

## CTempo
* This class manages the tempo and speed of music playback.
* It handles tempo changes, user-defined speed adjustments, and automatic tempo adjustments (experimental).

## CNote
* The CNote class represents a musical note with properties for pitch, duration, and the hand (left or right) to which it is assigned.
* It includes methods for transposing the note, determining the hand for a MIDI event based on channel and pitch, and managing the assignment of MIDI channels to specific hands.
* The class also provides static variables to track the active hand, MIDI channel assignments for left and right hands, and an array to store MIDI track information for each channel.
* This class is used within the CChord and CConductor classes to manage and play musical notes. It helps determine which hand a note belongs to, whether it falls within the playable range of a piano, and whether it should be played based on the current settings.
