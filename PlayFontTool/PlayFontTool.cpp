//********************************************************************************************************************************
// FontGrabber:		A very basic  utility for generating font sprite sheets compatible with PlayBuffer.
// Notes:			Grabs the glyphs for ASCII codes 32-126 [space to ~] and saves them to a 10x10 frame sprite image.
//					Make sure you have the appropriate rights to be able to redistribute your chosen font as a bitmap image.
//					Google provide a lot of open source fonts which can be used in this way: https://developers.google.com/fonts.
// Editing:			FontGrabber writes the glyph widths into an extra bottom row of pixels within the image itself.
//					If you are editing the font sprite sheet in an image editor then be careful not to change this final row!
// Controls:		Up/Down will change the pixel height of the font and [ / ] will increase and decrease glyph padding.
//					S will save the image to your desktop in the form: "fontnameXXpx_10x10.png", where XX is the pixel height.
// 
//											Copyright 2020 Sumo Digital Limited
//	   C++ code is made available under the Creative Commons Attribution - No Derivatives 4.0 International Public License 
//									https://creativecommons.org/licenses/by-nd/4.0/legalcode
//********************************************************************************************************************************

#define PLAY_IMPLEMENTATION
#include "Play.h"
#include "Commdlg.h"

const int DISPLAY_WIDTH = 1280;
const int DISPLAY_HEIGHT = 720;
const int DISPLAY_SCALE = 1;
const int FONT_NAME_Y = DISPLAY_HEIGHT - 32;
const int SAVE_PROMPT_Y = 16;
const int PANGRAM_Y = DISPLAY_HEIGHT - 100;
const Play::Colour BACKGROUND_COLOUR { 0.0f, 0.0f, 100.0f };

// This defines the characters we want in the font: PlayBuffer expects the alphanumeric characters to align with ASCII codes 32-126 by default
std::string g_charSet = { " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };

int g_glyphPadding = 2;
PixelData g_spriteSheet = { 0 };
char g_fontName[ 64 ] = { 0 };
int g_fontPixelHeight = 0;
int g_glyphYOffset = 0;
int g_fontWeight = FW_DONTCARE;
DWORD g_fontItalic = FALSE;
string g_fontStyle;
bool g_quitImmediately;

// Forward declaration
void CreateFontSpriteSheet( int pixelHeight, std::string fontName, PixelData& pixelData );

bool ChooseWindowsFont()
{
	CHOOSEFONT cf;
	LOGFONT lf;
	wchar_t fontStyle[64] = {0};
	char fontStyle_char[64] = {0};

	memset(&cf, 0, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.Flags = CF_SCALABLEONLY | CF_USESTYLE;
	cf.lpLogFont = &lf;
	cf.lpszStyle = fontStyle;

	// Throw up the font selection dialog - the point size is interpreted as a pixel height
	bool result = ChooseFont(&cf);
	PLAY_ASSERT_MSG(result, "You must select a font to continue");
	if (result == false)
	{
		return false;
	}
	PLAY_ASSERT_MSG(cf.lpLogFont->lfFaceName[0] != 0, "Not a valid font");

	// Grab the font at its initial size
	g_fontPixelHeight = cf.iPointSize / 10; // cf.iPointSize is in 1/10 point units
	WideCharToMultiByte(CP_ACP, 0, cf.lpLogFont->lfFaceName, -1, g_fontName, 32, NULL, NULL);

	WideCharToMultiByte(CP_ACP, 0, fontStyle, -1, fontStyle_char, 32, NULL, NULL);
	g_fontStyle = string(fontStyle_char);

	g_fontItalic = lf.lfItalic;
	g_fontWeight = lf.lfWeight;

	g_glyphPadding = 2;
	g_glyphYOffset = 0;
	return true;
}

// The entry point for a Windows program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	// Create the PlayBuffer manager : has its own built-in font!
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );

	// Open a system dialog to ask the player to select a font that is already installed on their PC.
	if (ChooseWindowsFont() == false)
	{
		g_quitImmediately = true;
	}
	else
	{
		CreateFontSpriteSheet(g_fontPixelHeight, g_fontName, g_spriteSheet);
		int fId = Play::Graphics::AddSprite("GRABBED", g_spriteSheet);
	}
}

// Called by the PlayBuffer once for each frame of the game (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	// Quit immediately.
	if (g_quitImmediately)
		return true;

	// Simple logic to allow the font to be dragged around the viewing area
	static Point2D fontPos = { 0, DISPLAY_HEIGHT * 2 / 3 };
	static Point2D clickPos = { 0, 0 };
	static bool drag = false;
	static Vector2D dragVector = { 0, 0 };

	if( Play::GetMouseButton( Play::LEFT ) )
	{
		if( drag )
		{
			dragVector = Play::GetMousePos() - clickPos;
		}
		else
		{
			drag = true;
			clickPos = Play::GetMousePos();
		}
	}
	else
	{
		if( drag )
		{
			fontPos += dragVector;
			dragVector = { 0, 0 };
			drag = false;
		}
	}

	// Allow the size and padding to be changed
	bool recreate = false;

	if( Play::KeyPressed( Play::KEY_PLUS ) )
	{
		g_fontPixelHeight++;
		recreate = true;
	}

	if( Play::KeyPressed( Play::KEY_MINUS ) )
	{
		g_fontPixelHeight--;
		recreate = true;
	}

	if( Play::KeyPressed( Play::KEY_UP ) )
	{
		g_glyphYOffset--;
		recreate = true;
	}

	if( Play::KeyPressed( Play::KEY_DOWN ) )
	{
		g_glyphYOffset++;
		recreate = true;
	}


	if( Play::KeyPressed( Play::KEY_OPEN_SQB ) )
	{
		g_glyphPadding--;
		recreate = true;
	}

	if( Play::KeyPressed( Play::KEY_CLOSE_SQB ) )
	{
		g_glyphPadding++;
		recreate = true;
	}

	if ( Play::KeyPressed(KEY_L) )
	{
		// Open a system dialog to ask the player to select a font that is already installed on their PC.
		ChooseWindowsFont();
		recreate = true;
	}

	// Regenerate the sprite sheet when the parameters change
	if( recreate )
	{
		CreateFontSpriteSheet( g_fontPixelHeight, g_fontName, g_spriteSheet );
		Play::Graphics::UpdateSprite( "GRABBED", g_spriteSheet );
	}

	// Save the font spritesheet to the desktop when the S key is pressed
	static int saveTimeout = 0;
	static std::string pngFilePath = "";
	saveTimeout--;

	if( Play::KeyPressed( Play::KEY_S ) && saveTimeout < 0 )
	{
		// Get the path to the user's desktop folder
		char desktopPath[ 1024 ];
		SHGetFolderPathA( NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath );

		std::string pngFileName = string( g_fontName ) + " (" + g_fontStyle + ")_" + std::to_string( g_fontPixelHeight ) + "px_10x10.png"; // Change this to your desired filename
		
		OPENFILENAMEA ofn;
		char fileName[MAX_PATH] = "";
		strcpy_s(fileName, pngFileName.c_str());

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = "PNG files (*.png)\0*.png";
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER;
		ofn.lpstrDefExt = "";
		ofn.lpstrInitialDir = desktopPath;
	
		if (GetSaveFileNameA(&ofn))
		{
			pngFilePath = ofn.lpstrFile;
			SavePNGImage( pngFilePath, g_spriteSheet );
			saveTimeout = 20;
		}
	}

	Play::ClearDrawingBuffer( BACKGROUND_COLOUR );

	// Display info about the font
	Play::DrawRect( { 0, FONT_NAME_Y }, { DISPLAY_WIDTH, FONT_NAME_Y + 16 }, cBlack, true );
	std::string style;


	Play::DrawDebugText( { DISPLAY_WIDTH / 2, FONT_NAME_Y + 8 }, (string( g_fontName ) + " (" + g_fontStyle + ") " + std::to_string( g_fontPixelHeight ) + " : PAD " + std::to_string( g_glyphPadding ) + +" : YOFF " + std::to_string( g_glyphYOffset )).c_str(), Play::cYellow, true);

	// Get PlayBuffer to treat the font as if it is a single frame and display it on screen
	int fId = Play::Graphics::UpdateSprite( "GRABBED", g_spriteSheet ); 
	Play::Graphics::SetSpriteOrigin( fId, { 0, Play::Graphics::GetSpriteSize( fId ).height } );
	Play::DrawSprite( fId, fontPos + dragVector, 0 );

	// Get PlayBuffer to treat the font as if each frame is a separate character and draw some text with respect to glyph boundary lines
	Play::Graphics::UpdateSprite( "GRABBED", g_spriteSheet, 10, 10 );
	Play::Graphics::CentreSpriteOrigin( fId );
	Play::DrawLine( { 0, PANGRAM_Y - (g_fontPixelHeight / 2) }, { DISPLAY_WIDTH, PANGRAM_Y - (g_fontPixelHeight / 2) }, cCyan );
	Play::DrawLine( { 0, PANGRAM_Y + (g_fontPixelHeight / 2) }, { DISPLAY_WIDTH, PANGRAM_Y + (g_fontPixelHeight / 2) }, cCyan );
	Play::DrawFontText( "GRABBED", "Five, joyful wizards quickly vexed the gaming goblins!", { DISPLAY_WIDTH / 2, PANGRAM_Y }, Play::CENTRE );
	
	// Display the controls or save message
	if( saveTimeout > 0 )
	{
		Play::DrawRect( { 0, SAVE_PROMPT_Y }, { DISPLAY_WIDTH, SAVE_PROMPT_Y + 16 }, cYellow, true );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, SAVE_PROMPT_Y + 8 }, string( "Saving: " + pngFilePath ).c_str(), Play::cBlack, true );
	}
	else
	{ 
		Play::DrawRect( { 0, SAVE_PROMPT_Y }, { DISPLAY_WIDTH, SAVE_PROMPT_Y + 16 }, cBlack, true );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, SAVE_PROMPT_Y + 8 }, "Plus and minus change the font size : [ and ] change the glyph padding : up and down position the glyph on the line : S save the font : L loads a new font", Play::cWhite, true );
	}
	
	Play::PresentDrawingBuffer();
	
	return Play::KeyDown( Play::KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void CreateFontSpriteSheet( int pixelHeight, std::string fontName, PixelData& pixelData )
{
	// Free up the previous pixel data, if this pixel data has been reused
	if( pixelData.pPixels != nullptr )
		delete[] pixelData.pPixels;

	pixelData.pPixels = nullptr;

	// Windows handles
	HDC hdc = CreateCompatibleDC( NULL );
	HFONT hFont = NULL;

	GLYPHMETRICS glyphMetrics = {};
	MAT2 mat{ {0, 1}, {0, 0}, {0, 0}, {0, 1} }; // 2D identity matrix

	// Different fonts can produce very different pixel sizes for the same point size.
	// So we begin by finding a font size with the desired pixel height for standard uppercase chars 

	int emHeight = pixelHeight / 2; // start small and work upwards
	int maxGlyphHeight = 0;

	// Define the characters used to determine the pixel height of the font
	std::string upperCase = { "ABCDEHIKMNOPRSTUVWXZ" };

	// Keep looping through until we've found the right glyph height for our needs
	while( maxGlyphHeight < pixelHeight )
	{
		maxGlyphHeight = 0;

		if( hFont )
			DeleteObject( hFont );

		hFont = CreateFontA( -emHeight, 0, 0, 0, g_fontWeight, g_fontItalic, TRUE, FALSE, ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, fontName.c_str() );
		PLAY_ASSERT_MSG( hFont, "Error creating font (wrong name?)" );
		SelectObject( hdc, hFont );

		// Find out the maximum glyph size for all the characters
		for( int glyphIndex = 0; glyphIndex < static_cast<int>(upperCase.size()); ++glyphIndex )
		{
			if( GetGlyphOutlineA( hdc, upperCase[ glyphIndex ], GGO_GRAY8_BITMAP, &glyphMetrics, 0, NULL, &mat ) > 0 )
			{
				if( glyphMetrics.gmBlackBoxY > (unsigned)maxGlyphHeight )
					maxGlyphHeight = glyphMetrics.gmBlackBoxY;
			}
		}

		PLAY_ASSERT_MSG( maxGlyphHeight > 0, "No glyph data available" );

		emHeight++;
	}

	// Next we get the maximum glyph size for all characters required in the font so that there's room for everything in the sprite sheet
	int maxGlyphWidth = 0;
	int maxGlyphY = 0;
	int minGlyphY = 9999;
	int maxGlyphX = 0;
	int minGlyphX = 9999;

	// Find out the maximum glyph size and offsets
	for( int glyphIndex = 0; glyphIndex < g_charSet.size(); ++glyphIndex )
	{
		int charInSet = g_charSet[ glyphIndex ];

		if( GetGlyphOutlineA( hdc, charInSet, GGO_GRAY8_BITMAP, &glyphMetrics, 0, NULL, &mat ) > 0 )
		{
			int glyphWidth = (glyphMetrics.gmBlackBoxX + 4 - 1) & ~3; // Rounded to nearest 4 bytes

			if( glyphWidth > maxGlyphWidth )
				maxGlyphWidth = glyphWidth;

			int glyphHeight = glyphMetrics.gmBlackBoxY;

			if( glyphHeight > maxGlyphHeight )
				maxGlyphHeight = glyphHeight;

			if( glyphMetrics.gmptGlyphOrigin.y > maxGlyphY )
				maxGlyphY = glyphMetrics.gmptGlyphOrigin.y;

			if( glyphMetrics.gmptGlyphOrigin.y < minGlyphY )
				minGlyphY = glyphMetrics.gmptGlyphOrigin.y;

			if( glyphMetrics.gmptGlyphOrigin.x > maxGlyphX )
				maxGlyphX = glyphMetrics.gmptGlyphOrigin.x;

			if( glyphMetrics.gmptGlyphOrigin.x < minGlyphX )
				minGlyphX = glyphMetrics.gmptGlyphOrigin.x;
		}
	}

	PLAY_ASSERT_MSG( maxGlyphHeight > 0, "No glyph data available" );

	// Some font glyphs extend above or below the line.
	if( minGlyphY < 0 )
		maxGlyphHeight += -minGlyphY;

	if( minGlyphX < 0 )
		maxGlyphWidth += -minGlyphX;

	// Include padding and any user offset in y position
	maxGlyphHeight += g_glyphPadding + abs( g_glyphYOffset );
	maxGlyphWidth += g_glyphPadding;

	// Allocate memory for sprite sheet image data
	int imageWidth = 10 * maxGlyphWidth;
	int imageHeight = (10 * maxGlyphHeight) + 1;
	PLAY_ASSERT_MSG( imageWidth >= 100, "Font sprite must be at least 100 pixel wide to accomodate glyph width data" );

	pixelData.width = imageWidth;
	pixelData.height = imageHeight;

	int imageSizeBytes = imageWidth * imageHeight;
	pixelData.pPixels = new Pixel[imageSizeBytes];
	memset( pixelData.pPixels, 0x00, imageSizeBytes*sizeof(Pixel) );
	Pixel* pGlyphWidthData = pixelData.pPixels + (imageWidth * (imageHeight-1)); // Extra line at the bottom of the sprite sheet is for the width data

	// Allocate memory for an idividual glyph image
	int glyphDataSize = maxGlyphWidth * maxGlyphHeight;
	char* pGlyphImage = new char[glyphDataSize];
	memset( pGlyphImage, 0, glyphDataSize );

	// Start in the top left glyph square
	int glyphTilePosX = 0;
	int glyphTilePosY = 0;

	// Iterate through the glyphs, copying the glyph image and width data to the font sprite sheet 
	for( int glyphIndex = 0; glyphIndex < g_charSet.size(); ++glyphIndex )
	{
		int charInSet = g_charSet[glyphIndex];
		int result = GetGlyphOutlineA( hdc, charInSet, GGO_GRAY8_BITMAP, &glyphMetrics, glyphDataSize, pGlyphImage, &mat );
		int glyphWidth = ( glyphMetrics.gmBlackBoxX + 4 - 1 ) & ~3; // Rounded to nearest 4 bytes
		int glyphHeight = glyphMetrics.gmBlackBoxY;

		if( GDI_ERROR != result )
		{
			// Calculate pixel offset within font sprite sheet
			Pixel* pRasterPos = pixelData.pPixels + ( glyphTilePosY * imageWidth + glyphTilePosX );
			
			// Adjust for the horizontal and vertical offset of different glyphs
			int offY = maxGlyphY - glyphMetrics.gmptGlyphOrigin.y + (g_glyphYOffset>0 ? g_glyphYOffset:0);
			pRasterPos += imageWidth * offY + glyphMetrics.gmptGlyphOrigin.x + g_glyphPadding;

			for( int i = 0; i < glyphHeight; ++i ) 
			{
				for( int j = 0; j < glyphWidth; ++j )
				{
					// Get greyscale alpha pixel data
					unsigned char alpha = (int)pGlyphImage[i * glyphWidth + j] * 255 / 64;
					
					if( alpha != 0x00 )
						pRasterPos[j] = alpha <<24 | 0x00FFFFFF; // Convert from 8 to 24 bit
				}
				pRasterPos += imageWidth;
			}

			// Write the character widths into the image data
			if( glyphWidth > 255 )
				glyphWidth = 255;

			// Write the glyph widths into the bottom line of the image data.
			// Appears as a black line - don't accidently alter it in an image editor!
			Pixel* pixel = pGlyphWidthData + g_charSet[ glyphIndex ] - 32;
			pixel->b = (uint8_t)glyphMetrics.gmCellIncX;
			pixel->a = 0xFF;

			// Make the space the same width as an 'n'
			if( g_charSet[ glyphIndex ] == 'n' )
			{
				pGlyphWidthData->b = (glyphWidth + 1) & 0xFF;
				pGlyphWidthData->a = 0xFF;
			}
		}

		// Move to next tile
		glyphTilePosX += maxGlyphWidth;

		// Move down a row of tiles
		if( ( glyphTilePosX + maxGlyphWidth ) > imageWidth )
		{
			glyphTilePosX = 0;
			glyphTilePosY += maxGlyphHeight;
		}
	}

	delete[] pGlyphImage;
	DeleteDC( hdc );

	return;
}
