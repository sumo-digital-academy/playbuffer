//********************************************************************************************************************************
// File:		PlayGraphics.cpp
// Description:	Manages 2D graphics operations on a PixelData buffer 
// Platform:	Independent
// Notes:		Uses PNG format. The end of the filename indicates the number of frames e.g. "bat_4.png" or "tiles_10x10.png"
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

PlayGraphics* PlayGraphics::s_pInstance = nullptr;

//********************************************************************************************************************************
// Constructor / Destructor (Private)
//********************************************************************************************************************************

PlayGraphics::PlayGraphics( int bufferWidth, int bufferHeight, const char* path )
{
	// A working buffer for our display. Each pixel is stored as an unsigned 32-bit integer: alpha<<24 | red<<16 | green<<8 | blue
	m_playBuffer.width = bufferWidth;
	m_playBuffer.height = bufferHeight;
	m_playBuffer.pPixels = new Pixel[static_cast<size_t>( bufferWidth ) * bufferHeight];
	m_playBuffer.preMultiplied = false;
	PLAY_ASSERT( m_playBuffer.pPixels );

	memset( m_playBuffer.pPixels, 0, sizeof( uint32_t ) * bufferWidth * bufferHeight );

	// Make the display buffer the render target for the blitter
	m_blitter.SetRenderTarget( &m_playBuffer );

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

				SetSpriteOrigin( spriteId, { originX, originY } );
			}

			png_infile.close();
		}
	}
}

PlayGraphics::~PlayGraphics()
{
	for( Sprite& s : vSpriteData )
	{
		if( s.canvasBuffer.pPixels )
			delete[] s.canvasBuffer.pPixels;

		if( s.preMultAlpha.pPixels )
			delete[] s.preMultAlpha.pPixels;
	}

	for( PixelData& pBgBuffer : vBackgroundData )
		delete[] pBgBuffer.pPixels;

	if( m_pDebugFontBuffer )
		delete[] m_pDebugFontBuffer;

	delete[] m_playBuffer.pPixels;
}

//********************************************************************************************************************************
// Instance functions
//********************************************************************************************************************************

PlayGraphics& PlayGraphics::Instance()
{
	PLAY_ASSERT_MSG( s_pInstance, "Trying to use PlayGraphics without initialising it!" );
	return *s_pInstance;
}

PlayGraphics& PlayGraphics::Instance( int bufferWidth, int bufferHeight, const char* path )
{
	PLAY_ASSERT_MSG( !s_pInstance, "Trying to create multiple instances of singleton class!" );
	s_pInstance = new PlayGraphics( bufferWidth, bufferHeight, path );
	return *s_pInstance;
}

void PlayGraphics::Destroy()
{
	PLAY_ASSERT_MSG( s_pInstance, "Trying to use destroy PlayGraphics when it hasn't been instanced!" );
	delete s_pInstance;
	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Loading functions
//********************************************************************************************************************************

int PlayGraphics::LoadSpriteSheet( const std::string& path, const std::string& filename )
{
	PixelData canvasBuffer;
	std::string spriteName = filename;
	int hCount = 1;
	int vCount = 1;

	// Switch everything to uppercase to avoid need to check case each time
	for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

	// Look for the final number in the filename to pull out the number of frames across the width
	size_t frameWidthEnd = spriteName.find_last_of( "0123456789" );
	size_t frameWidthStart = spriteName.find_last_not_of( "0123456789" );

	if( frameWidthEnd == spriteName.length() - 1 )
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
	PlayWindow::LoadPNGImage( fileAndPath, canvasBuffer ); // Allocates memory as we don't know the size
	
	return AddSprite( filename, canvasBuffer, hCount, vCount );
}

int PlayGraphics::AddSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
{
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
	vSpriteData.push_back( s );

	return s.id;
}

int PlayGraphics::UpdateSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
{
	// Switch everything to uppercase to avoid need to check case each time
	std::string spriteName = name;
	for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

	for( Sprite& s : vSpriteData )
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


int PlayGraphics::LoadBackground( const char* fileAndPath )
{
	// The background image may not be the right size for the background so we make sure the buffer is 
	PixelData backgroundImage;
	Pixel* pSrc, * pDest;

	Pixel* correctSizeBuffer = new Pixel[static_cast<size_t>( m_playBuffer.width ) * m_playBuffer.height];
	PLAY_ASSERT( correctSizeBuffer );

	std::string pngFile( fileAndPath );
	PLAY_ASSERT_MSG( std::filesystem::exists( fileAndPath ), "The background png does not exist at the given location." );
	PlayWindow::LoadPNGImage( pngFile, backgroundImage ); // Allocates memory in function as we don't know the size

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

	vBackgroundData.push_back( backgroundImage );

	return static_cast<int>( vBackgroundData.size() ) - 1;
}


//********************************************************************************************************************************
// Sprite Getters and Setters
//********************************************************************************************************************************
int PlayGraphics::GetSpriteId( const char* name ) const
{
	std::string tofind( name );
	for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

	for( const Sprite& s : vSpriteData )
	{
		if( s.name.find( tofind ) != std::string::npos )
			return s.id;
	}
	PLAY_ASSERT_MSG( false, "The sprite name is invalid!" );
	return -1;
}

const std::string& PlayGraphics::GetSpriteName( int spriteId )
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get name of invalid sprite id" );
	return vSpriteData[spriteId].name;
}

Vector2f PlayGraphics::GetSpriteSize( int spriteId ) const
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get width of invalid sprite id" );
	return { vSpriteData[spriteId].width, vSpriteData[spriteId].height };
}

int PlayGraphics::GetSpriteFrames( int spriteId ) const
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get frames of invalid sprite id" );
	return vSpriteData[spriteId].totalCount;
}

Vector2f PlayGraphics::GetSpriteOrigin( int spriteId ) const
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get origin with invalid sprite id" );
	return { vSpriteData[spriteId].originX, vSpriteData[spriteId].originY };
}

void PlayGraphics::SetSpriteOrigin( int spriteId, Vector2f newOrigin, bool relative )
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to set origin with invalid sprite id" );
	if( relative )
	{
		vSpriteData[spriteId].originX += static_cast<int>( newOrigin.x );
		vSpriteData[spriteId].originY += static_cast<int>( newOrigin.y );
	}
	else
	{
		vSpriteData[spriteId].originX = static_cast<int>( newOrigin.x );
		vSpriteData[spriteId].originY = static_cast<int>( newOrigin.y );
	}
}

void PlayGraphics::CentreSpriteOrigin( int spriteId )
{
	SetSpriteOrigin( spriteId, GetSpriteSize( spriteId ) / 2 );
}

void PlayGraphics::CentreAllSpriteOrigins( void )
{
	for( Sprite& s : vSpriteData )
		CentreSpriteOrigin( s.id );
}

void PlayGraphics::SetSpriteOrigins( const char* rootName, Vector2f newOrigin, bool relative )
{
	std::string tofind( rootName );
	for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

	for( Sprite& s : vSpriteData )
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

//********************************************************************************************************************************
// Drawing functions
//********************************************************************************************************************************

void PlayGraphics::DrawTransparent( int spriteId, Point2f pos, int frameIndex, float alphaMultiply ) const
{
	const Sprite& spr = vSpriteData[spriteId];
	int destx = static_cast<int>( pos.x + 0.5f ) - spr.originX;
	int desty = static_cast<int>( pos.y + 0.5f ) - spr.originY;
	frameIndex = frameIndex % spr.totalCount;
	int frameX = frameIndex % spr.hCount;
	int frameY = frameIndex / spr.hCount;
	int pixelX = frameX * spr.width;
	int pixelY = frameY * spr.height;
	int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

	m_blitter.BlitPixels( spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, alphaMultiply );
};

void PlayGraphics::DrawRotated( int spriteId, Point2f pos, int frameIndex, float angle, float scale, float alphaMultiply ) const
{
	const Sprite& spr = vSpriteData[spriteId];
	int destx = static_cast<int>( pos.x + 0.5f );
	int desty = static_cast<int>( pos.y + 0.5f );
	frameIndex = frameIndex % spr.totalCount;
	int frameX = frameIndex % spr.hCount;
	int frameY = frameIndex / spr.hCount;
	int pixelX = frameX * spr.width;
	int pixelY = frameY * spr.height;
	int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

	m_blitter.RotateScalePixels( spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, spr.originX, spr.originY, angle, scale, alphaMultiply );
}


void PlayGraphics::DrawBackground( int backgroundId )
{
	PLAY_ASSERT_MSG( m_playBuffer.pPixels, "Trying to draw background without initialising display!" );
	PLAY_ASSERT_MSG( vBackgroundData.size() > static_cast<size_t>(backgroundId), "Background image out of range!" );
	m_blitter.BlitBackground( vBackgroundData[backgroundId] );
}

void PlayGraphics::ColourSprite( int spriteId, int r, int g, int b )
{
	PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to colour invalid sprite id" );

	Sprite& s = vSpriteData[spriteId];
	uint32_t col = ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( b & 0xFF );

	PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, col );
	s.canvasBuffer.preMultiplied = true;
}

int PlayGraphics::DrawString( int fontId, Point2f pos, std::string text ) const
{
	PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );

	int width = 0;

	for( char c : text )
	{
		Draw( fontId, { pos.x + width, pos.y }, c - 32 );
		width += GetFontCharWidth( fontId, c );
	}
	return width;
}

int PlayGraphics::DrawStringCentred( int fontId, Point2f pos, std::string text ) const
{
	int totalWidth = 0;

	for( char c : text )
		totalWidth += GetFontCharWidth( fontId, c );

	pos.x -= totalWidth / 2;

	DrawString( fontId, pos, text );
	return totalWidth;
}

int PlayGraphics::DrawChar( int fontId, Point2f pos, char c ) const
{
	PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
	Draw( fontId, { pos.x, pos.y }, c - 32 );
	return GetFontCharWidth( fontId, c );
}

int PlayGraphics::DrawCharRotated( int fontId, Point2f pos, float angle, float scale, char c ) const
{
	PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
	DrawRotated( fontId, { pos.x, pos.y }, c - 32, angle, scale );
	return GetFontCharWidth( fontId, c );
}

int PlayGraphics::GetFontCharWidth( int fontId, char c ) const
{
	PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
	return (vSpriteData[fontId].canvasBuffer.pPixels + ( c - 32 ))->b; // character width hidden in pixel data
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
bool PlayGraphics::SpriteCollide( int id_1, Point2f pos_1, int frame_1, float angle_1, int s1PixelColl[4], int id_2, Point2f pos_2, int frame_2, float angle_2, int s2PixelColl[4] ) const
{
	//transform all co-ordinates of sprite2 into the frame of sprite 1.

	//To do this we'll set up three co-ordinate systems.
	//X,Y are screen, u,v are sprite 1 and a,b are sprite 2.
	//The input is position of centre of rotation in screen and position of centre of rotation in sprite1 and sprite2.

	//We define the origin of the sprite1 as the beginning of its buffer.
	//First I calculate the offset vector for sprite one in the screen.
	//in sprite offsetu = rotOffX, offsetv = rotOffY.
	//When I refer to origin of the sprite I mean the 0,0 pixel.


	//Next define corners of sprite
	const Sprite& s1 = vSpriteData[id_1];
	const Sprite& s2 = vSpriteData[id_2];

	//Convert collision box locations from relative to sprite origin to relative to sprite top left. Hence TL.
	int s1PixelCollTL[4]{ 0 };
	int s2PixelCollTL[4]{ 0 };

	for( int i{ 0 }; i < 2; i++ )
	{
		s1PixelCollTL[2 * i] = s1PixelColl[2 * i] + s1.originX;
		s1PixelCollTL[2 * i + 1] = s1PixelColl[2 * i + 1] + s1.originY;
		s2PixelCollTL[2 * i] = s2PixelColl[2 * i] + s2.originX;
		s2PixelCollTL[2 * i + 1] = s2PixelColl[2 * i + 1] + s2.originY;
	}

	//in screen
	float cosAngle1 = cos( angle_1 );
	float sinAngle1 = sin( angle_1 );
	float offsetSprite1X = cosAngle1 * s1.originX - sinAngle1 * s1.originY;
	float offsetSprite1Y = cosAngle1 * s1.originY + sinAngle1 * s1.originX;

	//Next I calculate the sprite origin in the screen.
	float originSprite1X = pos_1.x - offsetSprite1X;
	float originSprite1Y = pos_1.y - offsetSprite1Y;

	//Repeat for other sprite.
	float cosAngle2 = cos( angle_2 );
	float sinAngle2 = sin( angle_2 );
	float offsetSprite2X = cosAngle2 * s2.originX - sinAngle2 * s2.originY;
	float offsetSprite2Y = cosAngle2 * s2.originY + sinAngle2 * s2.originX;

	//Next I calculate the sprite origin in the screen.
	float originSprite2X = pos_2.x - offsetSprite2X;
	float originSprite2Y = pos_2.y - offsetSprite2Y;

	//calculate the difference between two sprite origins in screen
	float originDiffX = originSprite2X - originSprite1X;
	float originDiffY = originSprite2Y - originSprite1Y;

	//calculation of the difference between two sprite origins in frame of sprite 1.
	float originDiffu = originDiffX * cosAngle1 + originDiffY * sinAngle1;
	float originDiffv = originDiffY * cosAngle1 - originDiffX * sinAngle1;

	int s2Width = s2.width;
	int s2Height = s2.height;
	int s1Width = s1.width;

	float cosAngleDiff = cos( angle_2 - angle_1 );
	float sinAngleDiff = sin( angle_2 - angle_1 );
	//top left, top right, bottom right, bottom left.
	float s2Cu[4]
	{
		0,
		s2Width * cosAngleDiff,
		s2Width * cosAngleDiff - s2Height * sinAngleDiff,
		-s2Height * sinAngleDiff
	};

	float s2Cv[4]
	{
		0,
		s2Width * sinAngleDiff,
		s2Width * sinAngleDiff + s2Height * cosAngleDiff,
		s2Height * cosAngleDiff
	};


	float minCu{ std::numeric_limits<float>::infinity() };
	float minCv{ std::numeric_limits<float>::infinity() };
	float maxCu{ -std::numeric_limits<float>::infinity() };
	float maxCv{ -std::numeric_limits<float>::infinity() };

	//calculate the extremes of the rotated sprite2.
	for( int i = 0; i < 4; i++ )
	{
		minCu = std::min( s2Cu[i], minCu );
		minCv = std::min( s2Cv[i], minCv );
		maxCu = std::max( s2Cu[i], maxCu );
		maxCv = std::max( s2Cv[i], maxCv );
	}

	//calculate the maximum and minimum positions in the sprite1 frame of the sprite2 corners.
	float maxu = (originDiffu)+( maxCu );
	float maxv = (originDiffv)+( maxCv );
	float minu = (originDiffu)+( minCu );
	float minv = (originDiffv)+( minCv );

	if( minu > s1PixelCollTL[2] || minv > s1PixelCollTL[3] || maxu < s1PixelCollTL[0] || maxv < s1PixelCollTL[1] )
	{
		return false;
	}
	else
	{
		//wrap both frame indexes just in case.
		frame_1 = frame_1 % s1.totalCount;
		frame_2 = frame_2 % s2.totalCount;

		//clip so we loop through sprite 1.
		//Restrict the range we look in for pixel based collisions.
		minv = ( minv < s1PixelCollTL[1] ) ? static_cast<float>( s1PixelCollTL[1] ) : minv;
		maxv = ( maxv > s1PixelCollTL[3] ) ? static_cast<float>( s1PixelCollTL[3] ) : maxv;
		minu = ( minu < s1PixelCollTL[0] ) ? static_cast<float>( s1PixelCollTL[0] ) : minu;
		maxu = ( maxu > s1PixelCollTL[2] ) ? static_cast<float>( s1PixelCollTL[2] ) : maxu;

		//Set up the starting position in sprite 2 frame. We know the box corners in u, v relative to the sprite 2 origin but we need to get them in a,b.
		float minCa = ( minu - originDiffu ) * cosAngleDiff + ( minv - originDiffv ) * sinAngleDiff;
		float minCb = -( minu - originDiffu ) * sinAngleDiff + ( minv - originDiffv ) * cosAngleDiff;

		//rounding at this point
		int iminu = static_cast<int>( minu );
		int iminv = static_cast<int>( minv );
		int imaxu = static_cast<int>( maxu );
		int imaxv = static_cast<int>( maxv );

		float startinga = minCa;
		float startingb = minCb;

		float rowstarta = startinga;
		float rowstartb = startingb;

		//Set up starting and finishing pointers for both the sprite 1 buffer and sprite 2 buffer 
		//starting pointer for the sprite 1 buffer is the minu and minv.
		int sprite1Offset = s1Width * frame_1 + iminu + iminv * s1.canvasBuffer.width;
		Pixel* sprite1Src = s1.canvasBuffer.pPixels + sprite1Offset;

		//The base pointer for the sprite2 will just be start of the correct frame in the canvas buffer.
		int sprite2Offset = s2Width * frame_2;
		Pixel* sprite2Base = s2.canvasBuffer.pPixels + sprite2Offset;
		//Define the number which we need to add to get down a row in sprite1.
		int sprite1ChangeRow = s1.canvasBuffer.width - ( imaxu - iminu );

		//Start of double for loop.
		//Go through the overlapping region (warning may go out of the buffer of sprite 2.)
		for( int v{ iminv }; v < imaxv; v++ )
		{
			//store a and b to be the start of the row.
			float a = rowstarta;
			float b = rowstartb;

			for( int u{ iminu }; u < imaxu; u++ )
			{

				//Edit this part to add a collision box.
				//If we are in sprite 2 then extract the look at the pixels.
				if( a >= s2PixelCollTL[0] && b >= s2PixelCollTL[1] && a < s2PixelCollTL[2] && b < s2PixelCollTL[3] )
				{
					int sprite2Pixel = static_cast<int>( a ) + static_cast<int>( b ) * s2.canvasBuffer.width;
					Pixel sprite2Src = *( sprite2Base + sprite2Pixel );

					//If both pixels at that position are opaque then there is a collision. 
					if( sprite2Src.bits > 0x00FFFFFF && (*sprite1Src).bits > 0x00FFFFFF )
					{
						return true;
					}

				}
				//add change in for going along u. go along a row.
				a += cosAngleDiff;
				b += -sinAngleDiff;

				sprite1Src++;

			}
			//increment for row of sprite 1.
			sprite1Src += sprite1ChangeRow;

			//work out start of next row based on start of previous row. 
			rowstarta += sinAngleDiff;
			rowstartb += cosAngleDiff;
		}
	}
	return false;
}




//********************************************************************************************************************************
// Function:	PreMultiplyAlpha - calculates the (src*srcAlpha) alpha blending calculation in advance as it doesn't change
// Parameters:	s = the sprite to pre-calculate data for
// Notes:		Also inverts the alpha ready for the (dest*(1-srcAlpha)) calculation and stores information in the new
//				buffer which provides the number of fully-transparent pixels in a row (so they can be skipped)
//********************************************************************************************************************************
void PlayGraphics::PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply = 1.0f, Pixel colourMultiply = 0x00FFFFFF )
{
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



void PlayGraphics::DrawPixel( Point2f pos, Pixel srcPix )
{
	// Convert floating point co-ordinates to pixels
	m_blitter.DrawPixel( static_cast<int>( pos.x + 0.5f ), static_cast<int>( pos.y + 0.5f ), srcPix );
}

void PlayGraphics::DrawLine( Point2f startPos, Point2f endPos, Pixel pix )
{
	// Convert floating point co-ordinates to pixels
	int x1 = static_cast<int>( startPos.x + 0.5f );
	int y1 = static_cast<int>( startPos.y + 0.5f );
	int x2 = static_cast<int>( endPos.x + 0.5f );
	int y2 = static_cast<int>( endPos.y + 0.5f );

	m_blitter.DrawLine( x1, y1, x2, y2, pix );
}




void PlayGraphics::DrawRect( Point2f topLeft, Point2f bottomRight, Pixel pix, bool fill )
{
	// Convert floating point co-ordinates to pixels
	int x1 = static_cast<int>( topLeft.x + 0.5f );
	int x2 = static_cast<int>( bottomRight.x + 0.5f );
	int y1 = static_cast<int>( topLeft.y + 0.5f );
	int y2 = static_cast<int>( bottomRight.y + 0.5f );

	if( fill )
	{
		for( int x = x1; x < x2; x++ )
		{
			for( int y = y1; y < y2; y++ )
				m_blitter.DrawPixel( x, y, pix );
		}
	}
	else
	{
		m_blitter.DrawLine( x1, y1, x2, y1, pix );
		m_blitter.DrawLine( x2, y1, x2, y2, pix );
		m_blitter.DrawLine( x2, y2, x1, y2, pix );
		m_blitter.DrawLine( x1, y2, x1, y1, pix );
	}
}

// Private function called by DrawCircle
void PlayGraphics::DrawCircleOctants( int posX, int posY, int offX, int offY, Pixel pix )
{
	DrawPixel( { posX + offX , posY + offY }, pix );
	DrawPixel( { posX - offX , posY + offY }, pix );
	DrawPixel( { posX + offX , posY - offY }, pix );
	DrawPixel( { posX - offX , posY - offY }, pix );
	DrawPixel( { posX - offY , posY + offX }, pix );
	DrawPixel( { posX + offY , posY - offX }, pix );
	DrawPixel( { posX - offY , posY - offX }, pix );
	DrawPixel( { posX + offY , posY + offX }, pix );
}

void PlayGraphics::DrawCircle( Point2f pos, int radius, Pixel pix )
{
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

void PlayGraphics::DrawPixelData( PixelData* pixelData, Point2f pos, float alpha )
{
	if( !pixelData->preMultiplied )
	{
		PreMultiplyAlpha( pixelData->pPixels, pixelData->pPixels, pixelData->width, pixelData->height, pixelData->width );
		pixelData->preMultiplied = true;
	}
	m_blitter.BlitPixels( *pixelData, 0, static_cast<int>(pos.x), static_cast<int>(pos.y), pixelData->width, pixelData->height, alpha );
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
static const uint32_t debugFontData[] =
{
	0x87304178, 0x0800861F, 0xFFFFFFE1, 0x7BBFBE79, 0xF7FE79EF, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EF, 0xFFEE3DFE, 0x6BB86080,
	0x1078860F, 0xE3DFFEFE, 0x5BB7FEFB, 0xE7BE7BEF, 0xFFBFFF41, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFDFFEDF, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFEE3DDF, 0x7BB7FEFB, 0xE7BE7BEF, 0xFFFFFFFF, 0x7BB7BEF9, 0xE7BE7BEF,
	0xFFFFFFDF, 0x87B001F8, 0x187E87EF, 0xFFFFFFDF, 0x86106104, 0x00007A30, 0x1E7C5061, 0x79E79E79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E,
	0x59E79F79, 0xF7DF7B7F, 0x597DA79E, 0x58005F78, 0x30D0037F, 0x477DA79E, 0x59E79F79, 0xF7DE7B7F, 0x597DA79E, 0x59E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x41E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x7DE79F79,
	0xF7DE7B7F, 0x5E7DA79E, 0x7DE79E79, 0xF7DE7B7F, 0x5E7DA79E, 0x81E06104, 0x07C17A38, 0xDE01A7A1, 0x06106101, 0xE79E79E0, 0x337F3FFF, 0x79E79EED, 0xE79E79EF, 0xB77FBFFF, 0x79E79FED, 0xE79E79EF,
	0xB7BFBFFF, 0x79E79FED, 0xE79A79EF, 0xB7BFBFFF, 0x79E75FED, 0xE79AB5EF, 0x77DFBFFF, 0x05E0E1ED, 0xE79ACC0E, 0xF7DFBFFF, 0x7DE77EED, 0xE79AB7ED, 0xF7EFBFFF, 0x7DE7BEED, 0xE79A7BEB, 0xF7EFBFFF,
	0x7DE7BEED, 0xE79A7BE7, 0xF7F7BFFF, 0x7DA7BEED, 0xE79A7BE7, 0xF7F7BCFF, 0x7DD79EED, 0xEB5A7BE7, 0xF7FBBCFF, 0x7C27A1EE, 0x1CE57810, 0x33FB3EC0
};

void PlayGraphics::DecompressDubugFont( void )
{
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

int PlayGraphics::DrawDebugCharacter( Point2f pos, char c, Pixel pix )
{
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
				DrawPixel( { pos.x + x, pos.y + y }, pix );
		}
	}

	return FONT_CHAR_WIDTH;
}

int PlayGraphics::DrawDebugString( Point2f pos, const std::string& s, Pixel pix, bool centred )
{
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

int PlayGraphics::GetDebugStringWidth( const std::string& s )
{
	return static_cast<int>( s.length() ) * ( FONT_CHAR_WIDTH + 1 );
}

//********************************************************************************************************************************
// Timing bar functions
//********************************************************************************************************************************

LARGE_INTEGER PlayGraphics::EndTimingSegment()
{
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

int PlayGraphics::SetTimingBarColour( Pixel pix )
{
	TimingSegment newData;
	newData.pix = pix;
	newData.begin = EndTimingSegment().QuadPart;

	m_vTimings.push_back( newData );

	return static_cast<int>( m_vTimings.size() );
};

void PlayGraphics::DrawTimingBar( Point2f pos, Point2f size )
{
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

float PlayGraphics::GetTimingSegmentDuration( int id ) const
{
	PLAY_ASSERT_MSG( static_cast<size_t>(id) < m_vTimings.size(), "Invalid id for timing data." );
	return m_vTimings[id].millisecs;
}

void PlayGraphics::TimingBarBegin( Pixel pix )
{
	EndTimingSegment();
	m_vPrevTimings = m_vTimings;
	m_vTimings.clear();
	SetTimingBarColour( pix );
}