//********************************************************************************************************************************
// File:		PlayAudio.cpp
// Description:	Implementation of a very simple audio manager using XAudio2
// Platform:	Windows
// Notes:		Uses WAV format (uncompressed, so audio file sizes can be large)
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

using namespace std;

#define ASSERT_AUDIO PLAY_ASSERT_MSG( Play::Audio::m_bCreated, "Audio Manager not initialised. Call Audio::CreateManager() before using the Play::Audio library functions.")

namespace Play::Audio
{
	// Flag to record whether the manager has been created
	bool m_bCreated = false;

	// XAudio2 objects
	IXAudio2* m_pXAudio2 = nullptr;
	IXAudio2MasteringVoice* m_pMasterVoice = nullptr;
	std::mutex m_voiceMutex; // XAudio2 is multi-threaded so we will need to take precautions!

	// Each WAV file in the audio directory is loaded into a SoundEffect structure
	struct SoundEffect
	{
		std::string fileAndPath;
		uint8_t* pFileBuffer{ nullptr }; // The whole file is loaded into here
		bool isXWMA = false;
		XAUDIO2_BUFFER xAudio2Buffer{ 0 }; // Pointer to the WAV data within the file buffer
		XAUDIO2_BUFFER_WMA xAudio2BufferWMA{ 0 }; // Pointer to XWMA data.
		WAVEFORMATEXTENSIBLE format{ 0 }; 
	};
	std::vector< SoundEffect > m_vSoundEffects; // Vector of all the loaded sound effects

	// A new audio voice is created each time you play a sound effect
	struct AudioVoice
	{
		int id{ 0 };
		IXAudio2SourceVoice* pSourceVoice{ nullptr };
		SoundEffect* pSoundEffect{ nullptr };

		AudioVoice()
		{
			static int uniqueId = 0;
			id = uniqueId++;
		}
	};

	// A map is used internally to store all the playing sound effects and their unique ids
	static std::map<int, AudioVoice&> m_audioVoiceMap;

	// Internal (private) functions
	bool LoadSoundEffect( std::string& filename, SoundEffect& sf );
	bool DestroyVoice( int voiceId );
	
	// An XAudio2 callback is required to clean up audio voices when they have finished playing
	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart( UINT32 ) override {}
		void STDMETHODCALLTYPE OnBufferStart( void* ) override {}
		void STDMETHODCALLTYPE OnLoopEnd( void* ) override {}
		void STDMETHODCALLTYPE OnVoiceError( void*, HRESULT ) override {}
		void STDMETHODCALLTYPE OnBufferEnd( void* pBufferContext ) override { DestroyVoice( *((int*)pBufferContext) ); }
	};

	//********************************************************************************************************************************
	// Create and Destroy functions
	//********************************************************************************************************************************
	bool CreateManager( const char* path )
	{
		PLAY_ASSERT_MSG( !m_bCreated, "Audio manager has already been created!" );

		// Does the Audio folder exist?
		if (std::filesystem::is_directory(path)) {

			HRESULT hr;

			// Initialise XAudio2
			hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
			PLAY_ASSERT_MSG( hr == S_OK, "CoInitializeEx failed" );

			hr = XAudio2Create( &m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR );
			PLAY_ASSERT_MSG( hr == S_OK, "XAudio2Create failed" );

			hr = m_pXAudio2->CreateMasteringVoice( &m_pMasterVoice );
			PLAY_ASSERT_MSG( hr == S_OK, "CreateMasteringVoice failed" );

			// Iterate through the directory loading all the sound effects
			for( auto& p : std::filesystem::directory_iterator( path ) )
			{
				// Switch everything to uppercase to avoid need to check case each time
				std::string filename = p.path().string();
				for( char& c : filename ) c = static_cast<char>(toupper( c ));

				// Only load .wav files
				if( filename.find( ".WAV" ) != std::string::npos )
				{
					SoundEffect soundEffect;
					LoadSoundEffect( filename, soundEffect );
					m_vSoundEffects.push_back( soundEffect );
				}
			}
		}

		m_bCreated = true;
		return true;
	}

	bool DestroyManager( void )
	{
		ASSERT_AUDIO;

		// Stop and destroy all the voices
		while( m_audioVoiceMap.size() > 0 )
			DestroyVoice( m_audioVoiceMap.begin()->first );

		// Delete all the sound effects
		for( SoundEffect& soundEffect : m_vSoundEffects )
			delete[] soundEffect.pFileBuffer; // The XAudio2Buffer is within the pFileBuffer data

		// Close down XAudio2
		if( m_pMasterVoice )
			m_pMasterVoice->DestroyVoice();

		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Sound playing functions
	//********************************************************************************************************************************
	int StartSound( const char* name, bool bLoop, float volume ,float freqMod )
	{
		ASSERT_AUDIO;

		static VoiceCallback voiceCallback;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename( name );
		for( char& c : filename ) c = static_cast<char>(toupper( c ));

		// Iterate through the sound effect data looking for the requested effect
		for( SoundEffect& soundEffect : m_vSoundEffects )
		{
			if( soundEffect.fileAndPath.find( filename ) != std::string::npos )
			{
				m_voiceMutex.lock(); // XAudio2 is multi-threaded so we need to protect this code from being run in multiple threads at the same time.

				// Create a new voice to play the sound
				AudioVoice* pAudioVoice = new AudioVoice;
				m_pXAudio2->CreateSourceVoice( &pAudioVoice->pSourceVoice, (WAVEFORMATEX*)&soundEffect.format, 0u, 2.0f, &voiceCallback );
				pAudioVoice->pSoundEffect = &soundEffect;
				soundEffect.xAudio2Buffer.pContext = &pAudioVoice->id;
				soundEffect.xAudio2Buffer.LoopCount = bLoop ? XAUDIO2_LOOP_INFINITE : 0;
				pAudioVoice->pSourceVoice->SubmitSourceBuffer( &soundEffect.xAudio2Buffer, soundEffect.isXWMA? &soundEffect.xAudio2BufferWMA : 0);
				pAudioVoice->pSourceVoice->SetVolume( volume );
				pAudioVoice->pSourceVoice->SetFrequencyRatio( freqMod );
				pAudioVoice->pSourceVoice->Start( 0 );

				m_audioVoiceMap.insert( std::map<int, AudioVoice&>::value_type( pAudioVoice->id, *pAudioVoice ) );// Copy voice data pointer to the persistent map

				m_voiceMutex.unlock(); // Should be fine for concurrent threads to operate safely from this point
				return pAudioVoice->id;
			}
		}
		PLAY_ASSERT_MSG( false, std::string( "Trying to play unknown sound effect: " + std::string( name ) + "\nTry checking the 'Audio' folder").c_str());
		return -1;
	}

	bool StopSound( int voiceId )
	{
		ASSERT_AUDIO;
		return DestroyVoice( voiceId );
	}

	bool StopSound( const char* name )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename = name;
		for( char& c : filename ) c = static_cast<char>(toupper( c ));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
				return DestroyVoice( i.first );
		}
		return false;
	}

	void SetLoopingSoundVolume( const char* name, float volume )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename(name);
		for (char& c : filename) c = static_cast<char>(toupper(c));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
				i.second.pSourceVoice->SetVolume(volume);
		}
	}

	void SetLoopingSoundVolume(int voiceId, float volume)
	{
		ASSERT_AUDIO;

		// Try and find the voice to see if it exists
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return;

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;
		voice->pSourceVoice->SetVolume(volume);
	}

	void SetLoopingSoundPitch( const char* name, float freqMod )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename(name);
		for (char& c : filename) c = static_cast<char>(toupper(c));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
			{
				i.second.pSourceVoice->SetFrequencyRatio(freqMod);
			}
		}
	}

	void SetLoopingSoundPitch(int voiceId, float freqMod)
	{
		ASSERT_AUDIO;

		// Try and find the voice to see if it exists
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return;

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;
		voice->pSourceVoice->SetFrequencyRatio(freqMod);
	}

	bool LoadSoundEffect( std::string& filename, SoundEffect& soundEffect )
	{
		// RIFF (Resource Interchange File Format) is a tagged file structure for multimedia resource files. 
		// The RIFF structure identifies supported file formats using four-character codes, and groups their data into chunks. 
		struct RiffChunk
		{
			uint32_t m_id; // The type of data (4x char)
			uint32_t m_size; // The size of the chunk
		};

		// Open the file
		std::ifstream file;
		file.open( filename, std::ios::binary ); 
		PLAY_ASSERT( file.is_open() );

		// Get the size of file
		file.seekg( 0, std::ios::end );
		int fileSize = static_cast<int>(file.tellg());
		file.seekg( 0, std::ios::beg );

		// Allocate and read in the file
		soundEffect.pFileBuffer = new uint8_t[ fileSize ];
		file.read( (char*)soundEffect.pFileBuffer, fileSize );
		file.close();

		// Start working through the file data using a pointer
		const BYTE* p( static_cast<const BYTE*>(soundEffect.pFileBuffer) );
		const BYTE* pEnd( p + fileSize );

		// The first chunk is the root entry and must have a ID of 'RIFF' 
		const RiffChunk* pRiffChunk( reinterpret_cast<const RiffChunk*>(p) ); 
		PLAY_ASSERT_MSG( pRiffChunk->m_id == 'FFIR', "RIFF chunk invalid" ); // Reverse byte order = 'FFIR'
		p += sizeof( RiffChunk );


		// Next we're looking for a WAVE chunk (.WAV file) or a XWMA chunk (xWMA compressed audio).
		const uint32_t* pWave( reinterpret_cast<const uint32_t*>(p) ); // WAVE
		PLAY_ASSERT_MSG( *pWave == 'EVAW' || *pWave == 'AMWX', "WAVE/XWMA chunk invalid" );

		p += sizeof( uint32_t );

		// Subsequent chunks should be the sound data
		const RiffChunk* pChunk( reinterpret_cast<const RiffChunk*>(p) );
		p += sizeof( RiffChunk );

		bool bFoundFormat = false;
		bool bFoundData = false;
		bool bFounddpds = false;

		// Work through all the data
		while( p < pEnd )
		{
			switch( pChunk->m_id )
			{
			case ' tmf': // format chunk (fmt backwards)

				PLAY_ASSERT_MSG( sizeof( PCMWAVEFORMAT ) <= pChunk->m_size, "_fmt chunk invalid" );
				memcpy( &soundEffect.format, p, sizeof( PCMWAVEFORMAT ) );
				if (soundEffect.format.Format.wFormatTag == WAVE_FORMAT_WMAUDIO2 || soundEffect.format.Format.wFormatTag == WAVE_FORMAT_WMAUDIO3)
					soundEffect.isXWMA = true;
				bFoundFormat = true;
				break;

			case 'atad': // Data chunk (data backwards)
				soundEffect.xAudio2Buffer.pAudioData = p;
				soundEffect.xAudio2Buffer.AudioBytes = pChunk->m_size;
				bFoundData = true;
				break;
			case 'sdpd': // DPDS XWMA chunk 
				{
					if (soundEffect.isXWMA)
					{
						soundEffect.xAudio2BufferWMA.pDecodedPacketCumulativeBytes = reinterpret_cast<const uint32_t*>(p);
						soundEffect.xAudio2BufferWMA.PacketCount = pChunk->m_size / 4;
						bFounddpds = true;
					}
					break;
				}
			}

			p += pChunk->m_size;

			pChunk = reinterpret_cast<const RiffChunk*>(p);
			p += sizeof( RiffChunk );
		}

		
		PLAY_ASSERT_MSG( bFoundFormat && bFoundData, "Invalid Sound Data" );
		if (soundEffect.isXWMA)
			PLAY_ASSERT_MSG( bFounddpds, "Invalid XWMA sound")

		// Initialise typical flags (some overwritten on play)
		soundEffect.xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		soundEffect.xAudio2Buffer.LoopBegin = 0u;
		soundEffect.xAudio2Buffer.LoopLength = 0u;
		soundEffect.xAudio2Buffer.LoopCount = 0;
		soundEffect.fileAndPath = filename;

		return true;
	}

	bool DestroyVoice( int voiceId )
	{
		// Only delete audio voices that exist!
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return false;

		m_voiceMutex.lock(); // XAudio2 is multi-threaded so we need to protect this code from being run in multiple threads at the same time.

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;

		// Stop and destroy the audio voice
		voice->pSourceVoice->Stop();
		voice->pSourceVoice->FlushSourceBuffers();
		voice->pSoundEffect = nullptr;
		voice->pSourceVoice->DestroyVoice();
		m_audioVoiceMap.erase( m_audioVoiceMap.find( voiceId ) );
		delete voice;

		m_voiceMutex.unlock(); // Should be fine for concurrent threads to operate safely from this point

		return true;
	}
}