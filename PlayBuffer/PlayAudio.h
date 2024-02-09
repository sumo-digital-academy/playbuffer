#ifndef PLAY_PLAYAUDIO_H
#define PLAY_PLAYAUDIO_H
//********************************************************************************************************************************
// File:		PlayAudio.h
// Description:	Declarations for a simple audio manager 
// Platform:	Independant
//********************************************************************************************************************************
namespace Play::Audio
{
	// Initialises the audio manager, using the directory provided as its root for finding .WAV files
	bool CreateManager( const char* path );
	// Destroys any memory associated with the audio manager
	bool DestroyManager();
	// Play a sound using part of all of its filename, returns the voice id
	int StartSound( const char* name, bool bLoop = false, float volume = 1.0f, float freqMod = 1.0f);
	//  Stop a currently playing sound using its voice id
	bool StopSound( int voiceId ); 
	//  Stop all currently playing sounds with the same filename 
	bool StopSound( const char* name );
	// Set the volume of a looping sound that's playing using name
	void SetLoopingSoundVolume( const char* name, float volume = 1.0f);
	// Set the volume of a looping sound that's playing using id
	void SetLoopingSoundVolume( int voiceId, float volume = 1.0f);
	// Set the pitch of a looping sound that's playing using name
	void SetLoopingSoundPitch(const char* name, float freqMod = 1.0f);
	// Set the pitch of a looping sound that's playing using id
	void SetLoopingSoundPitch(int voiceId, float freqMod = 1.0f);
};
#endif // PLAY_PLAYAUDIO_H
