//********************************************************************************************************************************
// File:		PlayGraphics.cpp
// Description:	Manages 2D graphics operations on a PixelData buffer 
// Platform:	Independent
// Notes:		Uses PNG format. The end of the filename indicates the number of frames e.g. "bat_4.png" or "tiles_10x10.png"
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

#define ASSERT_GRAPHICS PLAY_ASSERT_MSG( Play::Graphics::m_bCreated, "Graphics Manager not initialised. Call Graphics::CreateManager() before using the Play::Graphics library functions.")

namespace Play::Graphics
{
	// Internal (private) declarations
	// 
	// Flag to record whether the manager has been created
	bool m_bCreated = false;
	// Count of the total number of sprites loaded
	int m_nTotalSprites{ 0 };
	// Buffer pointers
	PixelData m_playBuffer;
	// A buffer for unpacking the debug font into 
	uint8_t* m_pDebugFontBuffer{ nullptr };

	// A vector of all the loaded sprites
	std::vector< Sprite > m_vSpriteData;
	// A vector of all the loaded backgrounds
	std::vector< PixelData > m_vBackgroundData;

	// Multiplies the sprite image by its own alpha transparency values to save repeating this calculation on every draw
	// > A colour multiplication can also be applied at this stage, which affects all subseqent drawing operations on the sprite
	void PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply, Pixel colourMultiply );
	// Allocates a buffer for the debug font and copies the font pixel data to it
	void DecompressDubugFont( void );
	// Returns the pixel width of a string using the debug font
	int GetDebugStringWidth( const std::string& s );
	// Draws the offset points from the origin in all octants
	void DrawCircleOctants( int posX, int posY, int offX, int offY, Pixel pix );
	// Ends the current timing segment and calculates the duration
	LARGE_INTEGER EndTimingSegment();

	struct TimingSegment
	{
		Pixel pix;
		long long begin{ 0 };
		long long end{ 0 };
		float millisecs{ 0 };
	};

	// Vectors of timing data segments
	std::vector<TimingSegment> m_vTimings;
	std::vector<TimingSegment> m_vPrevTimings;

	// The blend mode state
	BlendMode blendMode{ BLEND_NORMAL };

	bool CreateManager( int bufferWidth, int bufferHeight, const char* path )
	{
		PLAY_ASSERT_MSG( !m_bCreated, "Graphics Manager already initialised! Cannot call Graphics::CreateManager() more than once.");

		m_bCreated = true;

		// A working buffer for our display. Each pixel is stored as an unsigned 32-bit integer: alpha<<24 | red<<16 | green<<8 | blue
		m_playBuffer.width = bufferWidth;
		m_playBuffer.height = bufferHeight;
		m_playBuffer.pPixels = new Pixel[static_cast<size_t>( bufferWidth ) * bufferHeight];
		m_playBuffer.preMultiplied = false;
		PLAY_ASSERT( m_playBuffer.pPixels );

		memset( m_playBuffer.pPixels, 0, sizeof( uint32_t ) * bufferWidth * bufferHeight );

		// Make the display buffer the render target for the blitter
		Render::SetRenderTarget( &m_playBuffer );

		// Iterate through the directory
		PLAY_ASSERT_MSG( std::filesystem::exists( path ), "PlayBuffer: Drectory provided does not exist." );

		for( const auto& p : std::filesystem::directory_iterator( path ) )
		{
			// Switch everything to uppercase to avoid need to check case each time
			std::string filename = p.path().string();
			for( char& c : filename ) c = static_cast<char>( toupper( c ) );

			// Only attempt to load PNG files
			if( filename.find( ".PNG" ) != std::string::npos )
			{
				std::ifstream png_infile;
				png_infile.open( filename, std::ios::binary ); // Don't do this as part of the constructor or we lose 16 bytes!

				// If the PNG was opened okay
				if( png_infile )
				{
					int spriteId = LoadSpriteSheet( p.path().parent_path().string() + "\\", p.path().stem().string() );

					// Now we check for .inf file for each sprite and load origins
					int originX = 0, originY = 0;

					std::string info_filename = filename.replace( filename.find( ".PNG" ), 4, ".INF" );

					if( std::filesystem::exists( info_filename ) )
					{
						std::ifstream info_infile;
						info_infile.open( info_filename, std::ios::in );

						PLAY_ASSERT_MSG( info_infile.is_open(), std::string( "Unable to load existing .inf file: " + info_filename ).c_str() );
						if( info_infile.is_open() )
						{
							std::string type;
							info_infile >> type;
							info_infile >> originX;
							info_infile >> originY;
						}
						info_infile.close();
					}
					SetSpriteOrigin( spriteId, { originX, originY }, false );
				}
				png_infile.close();
			}
		}
		return true;
	}

	bool DestroyManager()
	{
		ASSERT_GRAPHICS;

		for( Sprite& s : m_vSpriteData )
		{
			if( s.canvasBuffer.pPixels )
				delete[] s.canvasBuffer.pPixels;

			if( s.preMultAlpha.pPixels )
				delete[] s.preMultAlpha.pPixels;
		}

		for( PixelData& pBgBuffer : m_vBackgroundData )
			delete[] pBgBuffer.pPixels;

		if( m_pDebugFontBuffer )
			delete[] m_pDebugFontBuffer;

		delete[] m_playBuffer.pPixels;

		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Loading functions
	//********************************************************************************************************************************

	int LoadSpriteSheet( const std::string& path, const std::string& filename )
	{
		ASSERT_GRAPHICS;

		PixelData canvasBuffer;
		std::string spriteName = filename;
		bool isSpriteSheet = false;
		int hCount = 1;
		int vCount = 1;

		// Switch everything to uppercase to avoid need to check case each time
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		// Look for the final number in the filename to pull out the number of frames across the width
		size_t frameWidthEnd = spriteName.find_last_of( "0123456789" );
		size_t frameWidthStart = spriteName.find_last_not_of( "0123456789" );

		// Make sure we are a sprite sheet (valid forms are sprite_w and sprite_wXh)
		if (spriteName[frameWidthStart] == 'X' && frameWidthStart != 0)
		{
			size_t frameWidthDelimPos = spriteName.find_last_not_of("0123456789", frameWidthStart - 1);
			if (spriteName[frameWidthDelimPos] == '_')
				isSpriteSheet = true;
		}
		else if (spriteName[frameWidthStart] == '_')
		{
			isSpriteSheet = true;
		}

		if( frameWidthEnd == spriteName.length() - 1 && isSpriteSheet)
		{
			// Grab the number of frames
			std::string widthString = spriteName.substr( frameWidthStart + 1, frameWidthEnd - frameWidthStart );

			// Make sure the number is valid 
			size_t num = widthString.find_first_of( "0123456789" );
			PLAY_ASSERT_MSG( num == 0, std::string( "Incorrectly named sprite: " + filename ).c_str() );

			hCount = stoi( widthString );

			if( spriteName[frameWidthStart] == 'X' )
			{
				//Two dimensional sprite sheet so the width was actually the height: copy it over and work out the real width
				vCount = hCount;

				// Cut off the last number we just found plus an "x", then check for another number indicating a frame height (optional)
				std::string truncated = spriteName.substr( 0, frameWidthStart );
				frameWidthEnd = truncated.find_last_of( "0123456789" );
				frameWidthStart = truncated.find_last_not_of( "0123456789" );

				if( frameWidthEnd == truncated.length() - 1 && frameWidthStart != std::string::npos )
				{
					// Grab the number of images in the height
					widthString = truncated.substr( frameWidthStart + 1, frameWidthEnd - frameWidthStart );

					// Make sure the number is valid
					num = widthString.find_first_of( "0123456789" );
					PLAY_ASSERT_MSG( num == 0, std::string( "Incorrectly named sprite: " + filename ).c_str() );

					hCount = stoi( widthString );
				}
				else
				{
					PLAY_ASSERT_MSG( false, std::string( "Incorrectly named sprite: " + filename ).c_str() );
				}
			}
			else
			{
				vCount = 1;
			}
		}
		std::string fileAndPath( path + spriteName + ".PNG" );
		LoadPNGImage( fileAndPath, canvasBuffer ); // Allocates memory as we don't know the size
	
		return AddSprite( filename, canvasBuffer, hCount, vCount );
	}

	int AddSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
	{
		ASSERT_GRAPHICS;

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		Sprite s;
		s.id = m_nTotalSprites++;
		s.name = spriteName;
		s.originX = s.originY = 0;
		s.hCount = hCount;
		s.vCount = vCount;
		s.canvasBuffer = pixelData; // copy including pointer to pixel data

		s.totalCount = s.hCount * s.vCount;
		s.width = s.canvasBuffer.width / s.hCount;
		s.height = s.canvasBuffer.height / s.vCount;

		// Create a separate buffer with the pre-multiplyied alpha
		s.preMultAlpha.pPixels = new Pixel[static_cast<size_t>( s.canvasBuffer.width ) * s.canvasBuffer.height];
		s.preMultAlpha.width = s.canvasBuffer.width;
		s.preMultAlpha.height = s.canvasBuffer.height;
		memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
		PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
		s.canvasBuffer.preMultiplied = true;

		// Add the sprite to our vector
		m_vSpriteData.push_back( s );
		return s.id;
	}

	int UpdateSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
	{
		ASSERT_GRAPHICS; 

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( spriteName ) != std::string::npos )
			{
				// delete the old premultiplied buffer
				delete s.preMultAlpha.pPixels;

				s.hCount = hCount;
				s.vCount = vCount;
				s.canvasBuffer = pixelData; // copy including pointer to pixel data

				s.totalCount = s.hCount * s.vCount;
				s.width = s.canvasBuffer.width / s.hCount;
				s.height = s.canvasBuffer.height / s.vCount;

				// Create a new buffer with the pre-multiplyied alpha
				s.preMultAlpha.pPixels = new Pixel[static_cast<size_t>( s.canvasBuffer.width ) * s.canvasBuffer.height];
				s.preMultAlpha.width = s.canvasBuffer.width;
				s.preMultAlpha.height = s.canvasBuffer.height;
				memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
				PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
				s.canvasBuffer.preMultiplied = true;

				return s.id;
			}
		}
		return -1;
	}

	int UpdateSprite( const std::string& name )
	{
		ASSERT_GRAPHICS;

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>(toupper( c ));

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( spriteName ) != std::string::npos )
			{
				memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
				PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
				s.canvasBuffer.preMultiplied = true;

				return s.id;
			}
		}
		return -1;
	}


	int LoadBackground( const char* fileAndPath )
	{
		ASSERT_GRAPHICS;

		// The background image may not be the right size for the background so we make sure the buffer is 
		PixelData backgroundImage;
		Pixel* pSrc, * pDest;

		Pixel* correctSizeBuffer = new Pixel[static_cast<size_t>( m_playBuffer.width ) * m_playBuffer.height];
		PLAY_ASSERT( correctSizeBuffer );

		std::string pngFile( fileAndPath );
		PLAY_ASSERT_MSG( std::filesystem::exists( fileAndPath ), "The background png does not exist at the given location." );
		LoadPNGImage( pngFile, backgroundImage ); // Allocates memory in function as we don't know the size

		pSrc = backgroundImage.pPixels;
		pDest = correctSizeBuffer;

		//Copy the image to our background buffer clipping where necessary
		for( int h = 0; h < std::min( backgroundImage.height, m_playBuffer.height ); h++ )
		{
			for( int w = 0; w < std::min( backgroundImage.width, m_playBuffer.width ); w++ )
				*pDest++ = *pSrc++;

			// Skip pixels if we're clipping
			pDest += std::max( m_playBuffer.width - backgroundImage.width, 0 );
			pSrc += std::max( backgroundImage.width - m_playBuffer.width, 0 );
		}

		// Free up the loading buffer
		delete backgroundImage.pPixels;
		backgroundImage.pPixels = correctSizeBuffer;

		m_vBackgroundData.push_back( backgroundImage );

		return static_cast<int>( m_vBackgroundData.size() ) - 1;
	}

	//********************************************************************************************************************************
	// Sprite Getters and Setters
	//********************************************************************************************************************************
	int GetSpriteId( const char* name ) 
	{
		ASSERT_GRAPHICS;

		std::string tofind( name );
		for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

		for( const Sprite& s : m_vSpriteData )
		{
			if( s.name.find( tofind ) != std::string::npos )
				return s.id;
		}
		PLAY_ASSERT_MSG( false, "The sprite name is invalid!" );
		return -1;
	}

	const std::string& GetSpriteName( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get name of invalid sprite id" );
		return m_vSpriteData[spriteId].name;
	}

	Vector2f GetSpriteSize( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get width of invalid sprite id" );
		return { m_vSpriteData[spriteId].width, m_vSpriteData[spriteId].height };
	}

	int GetSpriteFrames( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get frames of invalid sprite id" );
		return m_vSpriteData[spriteId].totalCount;
	}

	Vector2f GetSpriteOrigin( int spriteId ) 
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get origin with invalid sprite id" );
		return { m_vSpriteData[spriteId].originX, m_vSpriteData[spriteId].originY };
	}

	void SetSpriteOrigin( int spriteId, Vector2f newOrigin, bool relative )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to set origin with invalid sprite id" );
		if( relative )
		{
			m_vSpriteData[spriteId].originX += static_cast<int>( newOrigin.x );
			m_vSpriteData[spriteId].originY += static_cast<int>( newOrigin.y );
		}
		else
		{
			m_vSpriteData[spriteId].originX = static_cast<int>( newOrigin.x );
			m_vSpriteData[spriteId].originY = static_cast<int>( newOrigin.y );
		}
	}

	void CentreSpriteOrigin( int spriteId )
	{
		ASSERT_GRAPHICS;
		SetSpriteOrigin( spriteId, GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreAllSpriteOrigins( void )
	{
		ASSERT_GRAPHICS;
		for( Sprite& s : m_vSpriteData )
			CentreSpriteOrigin( s.id );
	}

	void FlipSpriteOriginVertically( int spriteId )
	{
		ASSERT_GRAPHICS;
		Vector2f origin = GetSpriteOrigin( spriteId );
		origin.y = GetSpriteSize( spriteId ).y - origin.y;
		SetSpriteOrigin( spriteId, origin, false );
	}

	void FlipAllSpriteOriginsVertically( void )
	{
		ASSERT_GRAPHICS;
		for( Sprite& s : m_vSpriteData )
			FlipSpriteOriginVertically( s.id );
	}

	void SetSpriteOrigins( const char* rootName, Vector2f newOrigin, bool relative )
	{
		ASSERT_GRAPHICS;
		std::string tofind( rootName );
		for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( tofind ) != std::string::npos )
			{
				if( relative )
				{
					s.originX += static_cast<int>( newOrigin.x );
					s.originY += static_cast<int>( newOrigin.y );
				}
				else
				{
					s.originX = static_cast<int>( newOrigin.x );
					s.originY = static_cast<int>( newOrigin.y );
				}
			}
		}
	}

	int GetTotalLoadedSprites() 
	{ 
		ASSERT_GRAPHICS;
		return m_nTotalSprites; 
	}
	
	const PixelData* GetSpritePixelData(int spriteId) 
	{ 
		ASSERT_GRAPHICS;
		return &m_vSpriteData[spriteId].canvasBuffer; 
	}

	//********************************************************************************************************************************
	// Drawing functions
	//********************************************************************************************************************************

	void DrawTransparent( int spriteId, Point2f pos, int frameIndex, BlendColour globalMultiply)
	{
		ASSERT_GRAPHICS;
		const Sprite& spr = m_vSpriteData[spriteId];
		int destx = static_cast<int>( pos.x + 0.5f ) - spr.originX;
		int desty = static_cast<int>( pos.y + 0.5f ) + (spr.height - spr.originY);
		frameIndex = frameIndex % spr.totalCount;
		int frameX = frameIndex % spr.hCount;
		int frameY = frameIndex / spr.hCount;
		int pixelX = frameX * spr.width;
		int pixelY = frameY * spr.height;
		int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

		switch (blendMode)
		{
			case BLEND_NORMAL:
				Render::BlitPixels<Render::AlphaBlendPolicy>(spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			case BLEND_ADD:
				Render::BlitPixels<Render::AdditiveBlendPolicy>(spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			case BLEND_MULTIPLY:
				Render::BlitPixels<Render::MultiplyBlendPolicy>(spr.canvasBuffer, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			default:
				PLAY_ASSERT_MSG(false, "Unsupported blend mode in DrawTransparent")
					break;
		}

	};

	void DrawRotated( int spriteId, Point2f pos, int frameIndex, float angle, float scale, BlendColour globalMultiply )
	{
		ASSERT_GRAPHICS;
		Matrix2D trans = MatrixScale( scale, scale ) * MatrixRotation( angle )  * MatrixTranslation( pos.x, pos.y );
		DrawTransformed( spriteId, trans, frameIndex, globalMultiply);
	}

	void DrawTransformed( int spriteId, const Matrix2D& trans, int frameIndex, BlendColour globalMultiply)
	{
		ASSERT_GRAPHICS;
		const Sprite& spr = m_vSpriteData[spriteId];
		frameIndex = frameIndex % spr.totalCount;
		int frameX = frameIndex % spr.hCount;
		int frameY = frameIndex / spr.hCount;
		int pixelX = frameX * spr.width;
		int pixelY = frameY * spr.height;
		int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

		Vector2f origin = { spr.originX, spr.height - spr.originY };

		switch (blendMode)
		{
		case BLEND_NORMAL:
			Render::TransformPixels<Render::AlphaBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		case BLEND_ADD:
			Render::TransformPixels<Render::AdditiveBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		case BLEND_MULTIPLY:
			Render::TransformPixels<Render::MultiplyBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		default:
			PLAY_ASSERT_MSG(false, "Unsupported blend mode in DrawTransparent")
				break;
		}
	}

	void DrawBackground( int backgroundId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( m_playBuffer.pPixels, "Trying to draw background without initialising display!" );
		PLAY_ASSERT_MSG( m_vBackgroundData.size() > static_cast<size_t>(backgroundId), "Background image out of range!" );
		Render::BlitBackground( m_vBackgroundData[backgroundId] );
	}

	void ColourSprite( int spriteId, int r, int g, int b )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to colour invalid sprite id" );

		Sprite& s = m_vSpriteData[spriteId];
		uint32_t col = ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( b & 0xFF );

		PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, col );
		s.canvasBuffer.preMultiplied = true;
	}

	int DrawString( int fontId, Point2f pos, std::string text )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );

		int width = 0;

		for( char c : text )
		{
			Draw( fontId, { pos.x + width, pos.y }, c - 32 );
			width += GetFontCharWidth( fontId, c );
		}
		return width;
	}

	int DrawStringCentred( int fontId, Point2f pos, std::string text )
	{
		ASSERT_GRAPHICS;
		int totalWidth = 0;

		for( char c : text )
			totalWidth += GetFontCharWidth( fontId, c );

		pos.x -= totalWidth / 2;

		DrawString( fontId, pos, text );
		return totalWidth;
	}

	int DrawChar( int fontId, Point2f pos, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		Draw( fontId, { pos.x, pos.y }, c - 32 );
		return GetFontCharWidth( fontId, c );
	}

	int DrawCharRotated( int fontId, Point2f pos, float angle, float scale, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		DrawRotated( fontId, { pos.x, pos.y }, c - 32, angle, scale );
		return GetFontCharWidth( fontId, c );
	}

	int GetFontCharWidth( int fontId, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		int glyphWidthDataOffset = m_vSpriteData[ fontId ].canvasBuffer.width * (m_vSpriteData[ fontId ].canvasBuffer.height - 1);
		return (m_vSpriteData[fontId].canvasBuffer.pPixels + glyphWidthDataOffset + ( c - 32 ))->b; // character width hidden in pixel data
	}

	//********************************************************************************************************************************
	// Function:	SpriteCollide: function that checks by pixel if two sprites collide
	// Parameters:	s1Xpos, s1Ypos, s2Xpos, s2Ypos = the origin of rotation for both sprites.
	//				s1Id, s2Id = the ids of both sprites
	//				s1angle, s2angle = the angle of rotation for both sprites, clockwise. 0 = unrotated.
	//				s1Pixelcoll, s2Pixelcoll = the top left and bottom right co-ordinates defining the collision rectangle of both sprites
	//				
	// Returns: true if a single pixel or more overlap between the two sprites and false if not.
	// Notes:	rounding errors may cause it not to be pixel perfect.	
	//********************************************************************************************************************************
	int SpriteCollide( int spriteIdA, int frameIndexA, Matrix2D& transA, int spriteIdB, int frameIndexB, Matrix2D& transB )
	{
		ASSERT_GRAPHICS;
		int overlapping_pixels = 0;

		// This algorithm transforms Sprite A's pixels into the co-ordinate space of Sprite B and iterates through all Sprite A's pixels checking for collisions
		// This only works reliably if the pixels of Sprite A are smaller or the same size as those of Sprite B
		
		// If Matrix A's pixels are larger than Matrix A's in both dimensions then swap the sprites around
		if( transA.row[0].Length() > transB.row[ 0 ].Length() && transA.row[ 1 ].Length() > transB.row[ 1 ].Length() )
			return SpriteCollide( spriteIdB, frameIndexB, transB, spriteIdA, frameIndexA, transA );

		PLAY_ASSERT_MSG( transA.row[ 0 ].Length() <= transB.row[ 0 ].Length() && transA.row[ 1 ].Length() <= transB.row[ 1 ].Length(), "Sprite Collide algorithm only works with uniform scaling" );

		const Sprite& spr_a = m_vSpriteData[ spriteIdA ];
		const Sprite& spr_b = m_vSpriteData[ spriteIdB ];

		Matrix2D a_trans_right{ transA };
		a_trans_right.row[ 0 ] = { transA.row[ 0 ].x, transA.row[ 1 ].x, 0.0f };
		a_trans_right.row[ 1 ] = { transA.row[ 0 ].y, transA.row[ 1 ].y, 0.0f };
		a_trans_right.row[ 2 ] = { transA.row[ 2 ].x, -transA.row[ 2 ].y, 1.0f };
		
		Matrix2D b_trans_right{ transB };
		b_trans_right.row[ 0 ] = { transB.row[ 0 ].x, transB.row[ 1 ].x, 0.0f };
		b_trans_right.row[ 1 ] = { transB.row[ 0 ].y, transB.row[ 1 ].y, 0.0f };
		b_trans_right.row[ 2 ] = { transB.row[ 2 ].x, -transB.row[ 2 ].y, 1.0f };

		frameIndexA = frameIndexA % spr_a.totalCount;
		int a_frame_x = frameIndexA % spr_a.hCount;
		int a_frame_y = frameIndexA / spr_a.hCount;
		int a_pixel_x = a_frame_x * spr_a.width;
		int a_pixel_y = a_frame_y * spr_a.height;
		int a_frame_offset = a_pixel_x + (spr_a.canvasBuffer.width * a_pixel_y);

		frameIndexB = frameIndexB % spr_b.totalCount;
		int b_frame_x = frameIndexB % spr_b.hCount;
		int b_frame_y = frameIndexB / spr_b.hCount;
		int b_pixel_x = b_frame_x * spr_b.width;
		int b_pixel_y = b_frame_y * spr_b.height;
		int b_frame_offset = b_pixel_x + (spr_b.canvasBuffer.width * b_pixel_y);

		Vector2f a_origin = { spr_a.originX, spr_a.height - spr_a.originY };
		Vector2f b_origin = { spr_b.originX, spr_b.height - spr_b.originY };

		Matrix2D b_inv_trans = Play::MatrixTranslation( -b_origin.x, -b_origin.y ) * b_trans_right;
		b_inv_trans.Inverse();
		Matrix2D a2b_trans = Play::MatrixTranslation( -a_origin.x, -a_origin.y ) * a_trans_right * b_inv_trans;

		float b_posx = a2b_trans.row[2].x;
		float b_posy = a2b_trans.row[2].y;

		float b_xincx = a2b_trans.row[ 0 ].x;
		float b_xincy = a2b_trans.row[ 0 ].y;
		float b_yincx = a2b_trans.row[ 1 ].x;
		float b_yincy = a2b_trans.row[ 1 ].y;
		float b_xresetx = b_xincx * spr_a.width; // This needs to be sprite a's width as that's the space we are iterating through
		float b_xresety = b_xincy * spr_a.width; // This needs to be sprite a's width as that's the space we are iterating through

		uint32_t* a_pixel = (uint32_t*)spr_a.preMultAlpha.pPixels + a_frame_offset;
		uint32_t* a_pixel_end = a_pixel + (spr_a.height * spr_a.canvasBuffer.width);

		// Iterate sequentially through pixels within the render target buffer
		while( a_pixel < a_pixel_end )
		{
			// For each row of pixels in turn
			uint32_t* dst_row_end = a_pixel + spr_a.width;
			while( a_pixel < dst_row_end )
			{
				if( *a_pixel < 0xFF000000 )
				{
					// The origin of a pixel is in its centre
					int roundX = static_cast<int>(b_posx + 0.5f);
					int roundY = static_cast<int>(b_posy + 0.5f);

					// Clip within the sprite boundaries
					if( roundX >= 0 && roundY >= 0 && roundX < spr_b.width && roundY < spr_b.height )
					{
						int b_pixel_index = roundX + (roundY * spr_b.canvasBuffer.width);
						uint32_t* b_pixel = ((uint32_t*)spr_b.canvasBuffer.pPixels + b_pixel_index + b_frame_offset);
						if( *b_pixel & 0xFF000000 )
							overlapping_pixels++; // Could also overwite to visualise: *b_pixel = 0xFFFFFFFF, but need to call UpdateSprite afterwards.	
					}
				}

				// Move one horizontal pixel in render target, which corresponds to the x axis of the inverse matrix in sprite space
				a_pixel++;
				b_posx += b_xincx;
				b_posy += b_xincy;
			}

			// Move render target pointer to the start of the next row
			a_pixel += spr_a.canvasBuffer.width - spr_a.width;

			// Move sprite buffer pointer back to the start of the current row
			b_posx -= b_xresetx;
			b_posy -= b_xresety;

			// One vertical pixel in the render target corresponds to the y axis of the inverse matrix in sprite space
			b_posx += b_yincx;
			b_posy += b_yincy;
		}
		return overlapping_pixels;
	}

	//********************************************************************************************************************************
	// Function:	PreMultiplyAlpha - calculates the (src*srcAlpha) alpha blending calculation in advance as it doesn't change
	// Parameters:	s = the sprite to pre-calculate data for
	// Notes:		Also inverts the alpha ready for the (dest*(1-srcAlpha)) calculation and stores information in the new
	//				buffer which provides the number of fully-transparent pixels in a row (so they can be skipped)
	//********************************************************************************************************************************
	void PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply = 1.0f, Pixel colourMultiply = 0x00FFFFFF )
	{
		ASSERT_GRAPHICS;

		Pixel* pSourcePixels = source;
		Pixel* pDestPixels = dest;

		// Iterate through all the pixels in the entire canvas
		for( int bh = 0; bh < height; bh++ )
		{
			for( int bw = 0; bw < width; bw++ )
			{
				Pixel src = *pSourcePixels;

				// Separate the channels and calculate src*srcAlpha
				int srcAlpha = static_cast<int>( ( src.bits >> 24 ) * alphaMultiply );

				int destRed = ( srcAlpha * ( ( src.bits >> 16 ) & 0xFF ) ) >> 8;
				int destGreen = ( srcAlpha * ( ( src.bits >> 8 ) & 0xFF ) ) >> 8;
				int destBlue = ( srcAlpha * ( src.bits & 0xFF ) ) >> 8;

				destRed = ( destRed * ( ( colourMultiply.bits >> 16 ) & 0xFF ) ) >> 8;
				destGreen = ( destGreen * ( ( colourMultiply.bits >> 8 ) & 0xFF ) ) >> 8;
				destBlue = ( destBlue * ( colourMultiply.bits & 0xFF ) ) >> 8;

				srcAlpha = 0xFF - srcAlpha; // invert the alpha ready to multiply with the destination pixels
				*pDestPixels = ( srcAlpha << 24 ) | ( destRed << 16 ) | ( destGreen << 8 ) | destBlue;

				if( srcAlpha == 0xFF ) // Completely transparent pixel
				{
					int repeats = 0;
					// We can only skip to the end of the row because the sprite frames are arranged on a continuous canvas
					int maxSkip = maxSkipWidth - ( bw % maxSkipWidth );

					for( int zw = 1; zw < maxSkip; zw++ )
					{
						if( ( pSourcePixels + zw )->bits >> 24 == 0x00 ) // Another transparent pixel
							repeats++;
						else
							break;
					}
					*pDestPixels = 0xFF000000 | repeats; // Doesn't matter what the colour was so we use it to store the skip value
				}
				pDestPixels++;
				pSourcePixels++;
			}
		}
	}

	//********************************************************************************************************************************
	// Basic drawing functions
	//********************************************************************************************************************************
	void DrawPixel( Point2f pos, Pixel srcPix )
	{
		ASSERT_GRAPHICS;

		pos.y = Window::GetHeight() - pos.y; //// Flip the y-coordinate to be consistant with a Cartesian co-ordinate system

		// Convert floating point co-ordinates to pixels
		switch (blendMode)
		{
		case BLEND_NORMAL:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixelPreMult<Render::AlphaBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		case BLEND_ADD:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixelPreMult<Render::AdditiveBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		case BLEND_MULTIPLY:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixel<Render::MultiplyBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		default:
			PLAY_ASSERT_MSG(false, "Unsupported blend mode in PlayGraphics::DrawPixel")
				break;
		}
	}

	void DrawLine( Point2f startPos, Point2f endPos, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x1 = static_cast<int>( startPos.x + 0.5f );
		int y1 = static_cast<int>( startPos.y + 0.5f );
		int x2 = static_cast<int>( endPos.x + 0.5f );
		int y2 = static_cast<int>( endPos.y + 0.5f );

		Render::DrawLine( x1, y1, x2, y2, pix );
	}

	void DrawRect( Point2f bottomLeft, Point2f topRight, Pixel pix, bool fill /*= false */ )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x1 = static_cast<int>( bottomLeft.x + 0.5f );
		int x2 = static_cast<int>( topRight.x + 0.5f );
		int y1 = static_cast<int>( bottomLeft.y + 0.5f );
		int y2 = static_cast<int>( topRight.y + 0.5f );

		if( fill )
		{
			for( int x = x1; x < x2; x++ )
			{
				for( int y = y1; y < y2; y++ )
					DrawPixel({ x, y }, pix);
			}
		}
		else
		{
			Render::DrawLine( x1, y1, x2, y1, pix );
			Render::DrawLine( x2, y1, x2, y2, pix );
			Render::DrawLine( x2, y2, x1, y2, pix );
			Render::DrawLine( x1, y2, x1, y1, pix );
		}
	}

	// Private function called by DrawCircle
	void DrawCircleOctants( int posX, int posY, int offX, int offY, Pixel pix )
	{
		ASSERT_GRAPHICS;
		DrawPixel( { posX + offX , posY + offY }, pix );
		DrawPixel( { posX - offX , posY + offY }, pix );
		DrawPixel( { posX + offX , posY - offY }, pix );
		DrawPixel( { posX - offX , posY - offY }, pix );
		DrawPixel( { posX - offY , posY + offX }, pix );
		DrawPixel( { posX + offY , posY - offX }, pix );
		DrawPixel( { posX - offY , posY - offX }, pix );
		DrawPixel( { posX + offY , posY + offX }, pix );
	}

	void DrawCircle( Point2f pos, int radius, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x = static_cast<int>( pos.x + 0.5f );
		int y = static_cast<int>( pos.y + 0.5f );

		int dx = 0;
		int dy = radius;

		int d = 3 - 2 * radius;
		DrawCircleOctants( x, y, dx, dy, pix );

		while( dy >= dx )
		{
			dx++;
			if( d > 0 )
			{
				dy--;
				d = static_cast<int>( d + 4 * ( dx - dy ) + 10 );
			}
			else
			{
				d = static_cast<int>( d + 4 * dx + 6 );
			}
			DrawCircleOctants( x, y, dx, dy, pix );
		}
	};

	void DrawPixelData( PixelData* pixelData, Point2f pos, float alpha )
	{
		ASSERT_GRAPHICS;
		if( !pixelData->preMultiplied )
		{
			PreMultiplyAlpha( pixelData->pPixels, pixelData->pPixels, pixelData->width, pixelData->height, pixelData->width );
			pixelData->preMultiplied = true;
		}
		Render::BlitPixels<Render::AlphaBlendPolicy>(*pixelData, 0, static_cast<int>(pos.x), static_cast<int>(pos.y), pixelData->width, pixelData->height, { alpha, 1.0f, 1.0f, 1.0f });
	}

	//********************************************************************************************************************************
	// Debug font functions
	//********************************************************************************************************************************

	// Settings for the embedded debug font
	#define FONT_IMAGE_WIDTH	96
	#define FONT_IMAGE_HEIGHT	36
	#define FONT_CHAR_WIDTH		6
	#define FONT_CHAR_HEIGHT	12

	// A 96x36 font image @ 1bpp containing basic ASCII characters from 0x30 (0) to 0x5F (underscore) at 6x12 pixels each
	uint32_t debugFontData[] =
	{
		0x87304178, 0x0800861F, 0xFFFFFFE1, 0x7BBFBE79, 0xF7FE79EF, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EF, 0xFFEE3DFE, 0x6BB86080,	0x1078860F, 0xE3DFFEFE, 
		0x5BB7FEFB, 0xE7BE7BEF, 0xFFBFFF41, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFDFFEDF, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFEE3DDF, 0x7BB7FEFB, 0xE7BE7BEF, 0xFFFFFFFF, 0x7BB7BEF9, 0xE7BE7BEF,	0xFFFFFFDF, 0x87B001F8, 0x187E87EF, 0xFFFFFFDF, 
		0x86106104, 0x00007A30, 0x1E7C5061, 0x79E79E79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E,	0x59E79F79, 0xF7DF7B7F, 0x597DA79E, 0x58005F78, 0x30D0037F, 0x477DA79E, 
		0x59E79F79, 0xF7DE7B7F, 0x597DA79E, 0x59E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x41E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x7DE79F79,	0xF7DE7B7F, 0x5E7DA79E, 0x7DE79E79, 0xF7DE7B7F, 0x5E7DA79E, 0x81E06104, 0x07C17A38, 0xDE01A7A1, 
		0x06106101, 0xE79E79E0, 0x337F3FFF, 0x79E79EED, 0xE79E79EF, 0xB77FBFFF, 0x79E79FED, 0xE79E79EF,	0xB7BFBFFF, 0x79E79FED, 0xE79A79EF, 0xB7BFBFFF, 0x79E75FED, 0xE79AB5EF, 0x77DFBFFF, 0x05E0E1ED, 0xE79ACC0E, 0xF7DFBFFF, 
		0x7DE77EED, 0xE79AB7ED, 0xF7EFBFFF, 0x7DE7BEED, 0xE79A7BEB, 0xF7EFBFFF,	0x7DE7BEED, 0xE79A7BE7, 0xF7F7BFFF, 0x7DA7BEED, 0xE79A7BE7, 0xF7F7BCFF, 0x7DD79EED, 0xEB5A7BE7, 0xF7FBBCFF, 0x7C27A1EE, 0x1CE57810, 0x33FB3EC0
	};

	void DecompressDubugFont( void )
	{
		ASSERT_GRAPHICS;

		m_pDebugFontBuffer = new uint8_t[FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT];

		for( int y = 0; y < FONT_IMAGE_HEIGHT; y++ )
		{
			for( int x = 0; x < FONT_IMAGE_WIDTH; x++ )
			{
				int bufferIndex = ( y * FONT_IMAGE_WIDTH ) + x;
				int dataIndex = bufferIndex / 32;
				int dataShift = 31 - ( bufferIndex % 32 );

				// Convert 1bpp to an 8-bit array for easy data access
				if( ( debugFontData[dataIndex] >> dataShift ) & 0x01 )
					m_pDebugFontBuffer[( y * FONT_IMAGE_WIDTH ) + x] = 0; // not ARGB just 0 (no pixel)
				else
					m_pDebugFontBuffer[( y * FONT_IMAGE_WIDTH ) + x] = 1; // not ARGB just 1 (a pixel)
			}
		}
	}

	int DrawDebugCharacter( Point2f pos, char c, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Limited character set in the font (0x30-0x5F) so includes translation of useful chars outside that range
		switch( c )
		{
			case 0x2C: c = 0x5E; break; // Map full stop to comma (nearly the same!)
			case 0x2D: c = 0x3B; break; // Map minus to semi-colon (rarely used)
			case 0x28: c = 0x5B; break; // Map brackets to square brackets
			case 0x29: c = 0x5D; break; // Map brackets to square brackets
			case 0x2E: c = 0x5E; break; // Map full stop to carot (never used)
		}

		if( c < 0x30 || c > 0x5F )
			return FONT_CHAR_WIDTH;

		int sourceX = ( ( c - 0x30 ) % 16 ) * FONT_CHAR_WIDTH;
		int sourceY = ( ( c - 0x30 ) / 16 ) * FONT_CHAR_HEIGHT;

		// Loop over the bounding box of the glyph
		for( int x = 0; x < FONT_CHAR_WIDTH; x++ )
		{
			for( int y = 0; y < FONT_CHAR_HEIGHT; y++ )
			{
				if( m_pDebugFontBuffer[( ( sourceY + y ) * FONT_IMAGE_WIDTH ) + ( sourceX + x )] > 0 )
					DrawPixel( { pos.x + x, pos.y + (FONT_CHAR_HEIGHT - y - 1) }, pix ); 
			}
		}
		return FONT_CHAR_WIDTH;
	}

	int DrawDebugString( Point2f pos, const std::string& s, Pixel pix, bool centred )
	{
		ASSERT_GRAPHICS;

		if( m_pDebugFontBuffer == nullptr )
			DecompressDubugFont();

		if( centred )
			pos.x -= GetDebugStringWidth( s ) / 2;

		pos.y -= 6; // half the height of the debug font

		for( char c : s )
			pos.x += DrawDebugCharacter( pos, static_cast<char>( toupper( c ) ), pix ) + 1;

		// Return horizontal position at the end of the string so strings can be concatenated easily
		return static_cast<int>( pos.x );
	}

	int GetDebugStringWidth( const std::string& s )
	{
		ASSERT_GRAPHICS;
		return static_cast<int>( s.length() ) * ( FONT_CHAR_WIDTH + 1 );
	}

	//********************************************************************************************************************************
	// Miscellaneous functions
	//********************************************************************************************************************************
	PixelData* GetDrawingBuffer(void) 
	{ 
		ASSERT_GRAPHICS;
		return &m_playBuffer; 
	}

	LARGE_INTEGER EndTimingSegment()
	{
		ASSERT_GRAPHICS;

		int size = static_cast<int>( m_vTimings.size() );

		LARGE_INTEGER now, freq;
		QueryPerformanceCounter( &now );
		QueryPerformanceFrequency( &freq );

		if( size > 0 )
		{
			m_vTimings[size - 1].end = now.QuadPart;
			m_vTimings[size - 1].millisecs = static_cast<float>( m_vTimings[size - 1].end - m_vTimings[size - 1].begin );
			m_vTimings[size - 1].millisecs = static_cast<float>( ( m_vTimings[size - 1].millisecs * 1000.0f ) / freq.QuadPart );
		}
		return now;
	}

	int SetTimingBarColour( Pixel pix )
	{
		ASSERT_GRAPHICS;

		TimingSegment newData;
		newData.pix = pix;
		newData.begin = EndTimingSegment().QuadPart;

		m_vTimings.push_back( newData );

		return static_cast<int>( m_vTimings.size() );
	};

	void DrawTimingBar( Point2f pos, Point2f size )
	{
		ASSERT_GRAPHICS;

		EndTimingSegment();

		int startPixel{ 0 };
		int endPixel{ 0 };
		for( const TimingSegment& t : m_vPrevTimings )
		{
			endPixel += static_cast<int>( ( size.width * t.millisecs ) / 16.667f );
			DrawRect( { pos.x + startPixel, pos.y }, { pos.x + endPixel, pos.y + size.height }, t.pix, true );
			startPixel = endPixel;
		}

		DrawRect( { pos.x, pos.y }, { pos.x + size.width, pos.y + size.height }, PIX_BLACK, false );
		DrawRect( { pos.x - 1, pos.y - 1 }, { pos.x + size.width + 1 , pos.y + size.height + 1 }, PIX_WHITE, false );
	}

	float GetTimingSegmentDuration( int id )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( static_cast<size_t>(id) < m_vTimings.size(), "Invalid id for timing data." );
		return m_vTimings[id].millisecs;
	}

	void TimingBarBegin( Pixel pix )
	{
		ASSERT_GRAPHICS;
		EndTimingSegment();
		m_vPrevTimings = m_vTimings;
		m_vTimings.clear();
		SetTimingBarColour( pix );
	}
}