#ifndef PLAY_PLAYAUDIO_H
#define PLAY_PLAYAUDIO_H
//********************************************************************************************************************************
//* File:			PlayAudio.h
//* Description:	Declaration for a simple audio manager class
//********************************************************************************************************************************

// Encapsulates the functionality of a simple audio manager 
// > A singleton class accessed using PlayAudio::Instance()
class PlayAudio
{
public:
	// Instance access functions 
	//********************************************************************************************************************************

	// Instantiates class and loads all the .MP3 sounds from the directory provided
	static PlayAudio& Instance( const char* path );
	// Returns the PlaySpeaker instance
	static PlayAudio& Instance();
	// Destroys the PlaySpeaker instance
	static void Destroy();

	// Playing and stopping audio
	//********************************************************************************************************************************

	// Play a sound using part of all of its name
	void StartAudio( const char* name, bool bLoop );
	//  Stop the currently playing sound using part of all of its name
	void StopAudio( const char* name ); 

private:
	// Constructor and destructor
	//********************************************************************************************************************************

	// Creates manager object and loads all the .MP3 sounds in the specified directory
	PlayAudio( const char* path ); 
	// Destroys the manager and stops any sounds playing
	~PlayAudio(); 
	// The assignment operator is removed to prevent copying of a singleton class
	PlayAudio& operator=( const PlayAudio& ) = delete;
	// The copy operator is removed to prevent copying of a singleton class
	PlayAudio( const PlayAudio& ) = delete;

	// Vector of mp3 strings
	std::vector< std::string > vSoundStrings;
	// Pointer to the singleton
	static PlayAudio* s_pInstance;
};

#endif
