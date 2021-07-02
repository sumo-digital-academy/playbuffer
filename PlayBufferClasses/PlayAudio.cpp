//********************************************************************************************************************************
// File:		PlaySpeaker.cpp
// Description:	Implementation of a very simple audio manager using the MCI
// Platform:	Windows
// Notes:		Uses MP3 format. The Windows multimedia library is extremely basic, but very quick easy to work with. 
//				Playback isn't always instantaneous and can trigger small frame glitches when StartSound is called. 
//				Consider XAudio2 as a potential next step.
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

// Instruct Visual Studio to link the multimedia library  
#pragma comment(lib, "winmm.lib")

PlayAudio* PlayAudio::s_pInstance = nullptr;

//********************************************************************************************************************************
// Constructor and destructor (private)
//********************************************************************************************************************************
PlayAudio::PlayAudio( const char* path )
{
	PLAY_ASSERT_MSG( !s_pInstance, "PlayAudio is a singleton class: multiple instances not allowed!" );
	PLAY_ASSERT_MSG( std::filesystem::is_directory( path ), "Audio directory does not exist!" );

	// Iterate through the directory
	for( auto& p : std::filesystem::directory_iterator( path ) )
	{
		// Switch everything to uppercase to avoid need to check case each time
		std::string filename = p.path().string();
		for( char& c : filename ) c = static_cast<char>( toupper( c ) );

		// Only load .mp3 files
		if( filename.find( ".MP3" ) != std::string::npos )
		{
			vSoundStrings.push_back( filename );
			std::string command = "open \"" + filename + "\" type mpegvideo alias " + filename;
			mciSendStringA( command.c_str(), NULL, 0, 0 );
		}
	}

	s_pInstance = this;
}

PlayAudio::~PlayAudio( void )
{
	for( std::string& s : vSoundStrings )
	{
		std::string command = "close " + s;
		mciSendStringA( command.c_str(), NULL, 0, 0 );
	}

	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Instance access functions
//********************************************************************************************************************************

PlayAudio& PlayAudio::Instance()
{
	PLAY_ASSERT_MSG( s_pInstance, "Trying to use PlayAudio without initialising it!" );
	return *s_pInstance;
}

PlayAudio& PlayAudio::Instance( const char* path )
{
	PLAY_ASSERT_MSG( !s_pInstance, "Trying to create multiple instances of singleton class!" );
	s_pInstance = new PlayAudio( path );
	return *s_pInstance;
}

void PlayAudio::Destroy()
{
	PLAY_ASSERT_MSG( s_pInstance, "Trying to use destroy PlayAudio which hasn't been instanced!" );
	delete s_pInstance;
	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Sound playing functions
//********************************************************************************************************************************
void PlayAudio::StartAudio( const char* name, bool bLoop )
{
	std::string filename( name );
	for( char& c : filename ) c = static_cast<char>( toupper( c ) );

	// Iterate through the sound data 
	for( std::string& s : vSoundStrings )
	{
		if( s.find( filename ) != std::string::npos )
		{
			std::string command = "play " + s + " from 0";
			if( bLoop ) command += " repeat";
			mciSendStringA( command.c_str(), NULL, 0, 0 );
			return;
		}
	}
	PLAY_ASSERT_MSG( false, std::string( "Trying to play unknown sound effect: " + std::string( name ) ).c_str() );
}

void PlayAudio::StopAudio( const char* name )
{
	std::string filename( name );
	for( char& c : filename ) c = static_cast<char>( toupper( c ) );

	// Iterate through the sound data 
	for( std::string& s : vSoundStrings )
	{
		if( s.find( filename ) != std::string::npos )
		{
			std::string command = "stop " + s;
			mciSendStringA( command.c_str(), NULL, 0, 0 );
			return;
		}
	}
	PLAY_ASSERT_MSG( false, std::string( "Trying to stop unknown sound effect: " + std::string( name ) ).c_str() );
}