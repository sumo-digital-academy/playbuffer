#ifndef PLAY_PLAYBLENDS_H
#define PLAY_PLAYBLENDS_H
//********************************************************************************************************************************
// File:		PlayBlends.h
// Description:	A set of software pixel blending operations used by the templates in PlayRender.h
// Platform:	Independent
//********************************************************************************************************************************
namespace Play::Render
{
	// The pre-multiplied alpha buffers store the number of subsequent pixels which are also transparent 
	// so they can be skipped. This works for any blend mode which uses the pre-multiplied alpha buffer. 
	inline void Skip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
	{
		// If this is a fully transparent pixel then the low bits store how many there are in a row
		// This means we can skip straight to the next pixel which isn't fully transparent
		uint32_t skip = static_cast<uint32_t>(destRowEnd - destPixels) - 1;
		uint32_t src = *srcPixels & 0x00FFFFFF;
		if (skip > src) skip = src;
		srcPixels += skip + 1;
		++destPixels += skip;
	}

	class AlphaBlendPolicy
	{
	public:
		// Standard alpha blending using a pre-multiplied srcAlpha buffer: (src * srcAlpha)+(dest * (1-srcAlpha)
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
		{
			// Optimal alpha blend approach performed on all channels simultaneously! 
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above  
			if (BlendFast(srcPixels, destPixels))
				srcPixels++, destPixels++;
			else
				Skip(srcPixels, destPixels, destRowEnd);
		}

		// Standard alpha blending, but with an additional global alpha multiply
		static inline void BlendSkip( uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t* destRowEnd )
		{
			// A slower blend calculation is required for semi-transparent pixels with a global multiply
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above   
			if( Blend( srcPixels, destPixels, globalMultiply ) )
				srcPixels++, destPixels++;
			else
				Skip( srcPixels, destPixels, destRowEnd );
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs a 'typical' alpha blending operation: (src * srcAlpha)+(dest * (1-srcAlpha))
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Note: our source alpha values are already 1-srcAlpha (to make the optimal BlendFast approach faster) so we need to flip them back again
			uint32_t srcAlpha = static_cast<int>((0xFF - (src >> 24)) * globalMultiply.alpha);
			uint32_t constAlpha = static_cast<int>(0xFF * globalMultiply.alpha);

			// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
			uint32_t destRed = (uint32_t)(constAlpha * globalMultiply.red * ((src >> 16) & 0xFF));
			uint32_t destGreen = (uint32_t)(constAlpha * globalMultiply.green * ((src >> 8) & 0xFF));
			uint32_t destBlue = (uint32_t)(constAlpha * globalMultiply.blue * (src & 0xFF));

			uint32_t invSrcAlpha = 0xFF - srcAlpha;

			// Apply a standard Alpha blend [ src*srcAlpha + dest*(1-SrcAlpha) ]
			destRed += invSrcAlpha * ((dest >> 16) & 0xFF);
			destGreen += invSrcAlpha * ((dest >> 8) & 0xFF);
			destBlue += invSrcAlpha * (dest & 0xFF);

			// Bring back to the range 0-255
			destRed >>= 8;
			destGreen >>= 8;
			destBlue >>= 8;

			// Put ARGB components back together again
			*destPixels = 0xFF000000 | (destRed << 16) | (destGreen << 8) | destBlue;

			return true;
		}

		// *******************************************************************************************************************************************************
		// An optimized approach which uses pre-multiplied alpha, parallel channel multiplication and pixel skipping to achieve the same 'typical' alpha 
		// blending operation (src * srcAlpha)+(dest * (1-srcAlpha)). Not easy to apply a global alpha multiplication over the top, but used everywhere else.
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool BlendFast(uint32_t*& srcPixels, uint32_t*& destPixels)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			// This performs the dest*(1-srcAlpha) calculation for all channels in parallel with minor accuracy loss in dest colour.
			// It does this by shifting all the destination channels down by 4 bits in order to "make room" for the later multiplication.
			// After shifting down, it masks out the bits which have shifted into the adjacent channel data.
			// This causes the RGB data to be rounded down to their nearest 16 producing a reduction in colour accuracy for pixels with alpha
			// This is then multiplied by the inverse alpha (inversed in PreMultiplyAlpha), also divided by 16 (hence >> 8+8+8+4).
			// The multiplication brings our RGB values back up to their original bit ranges (albeit rounded to the nearest 16).
			// As the colour accuracy only affects the destination pixels behind semi-transparent source pixels thia isn't very obvious.
			uint32_t dest = (((*destPixels >> 4) & 0x000F0F0F) * (*srcPixels >> 28));
			// Add the (pre-multiplied Alpha) source to the destination and force alpha to opaque
			*destPixels = (*srcPixels + dest) | 0xFF000000;

			return true;
		}
	};

	class AdditiveBlendPolicy
	{
	public:
		// Standard additive blending using pre-multiplied srcAlpha buffer: src*srcAlpha + dest*destAlpha
		// This isn't actually a very common requirement, so we default to the same global multiply approach below 
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
		{
			BlendSkip( srcPixels, destPixels, { 1.0f, 1.0f, 1.0f, 1.0f }, destRowEnd );
		}

		// Standard additive blending, with a global alpha multiply. This is the most common requirement for particle effects.
		static inline void BlendSkip( uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t* destRowEnd )
		{
			// A full blend calculation is required for semi-transparent pixels with a global multiply
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above   
			if( Blend( srcPixels, destPixels, globalMultiply ) )
				srcPixels++, destPixels++;
			else
				Skip( srcPixels, destPixels, destRowEnd );
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs an additive blending operation: (src * srcAlpha)+(dest * destAlpha)
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Keep the blended alpha calculation in the high bits
			uint32_t destAlpha = dest >> 24;
			uint32_t srcAlpha = 0xFF - (src >> 24);
			uint32_t blendedAlpha = srcAlpha + destAlpha;

			// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
			uint32_t blendedRed = (uint32_t)( globalMultiply.alpha * globalMultiply.red * ((src >> 8) & 0xFF00));
			uint32_t blendedGreen = (uint32_t)( globalMultiply.alpha * globalMultiply.green * (src & 0xFF00));
			uint32_t blendedBlue = (uint32_t)( globalMultiply.alpha * globalMultiply.blue * ((src << 8) & 0xFF00));

			// Apply an additive blend [ src*srcAlpha + dest*destAlpha ]
			blendedRed += 0xFF * ((dest >> 16) & 0xFF);
			blendedGreen += 0xFF * ((dest >> 8) & 0xFF);
			blendedBlue += 0xFF * (dest & 0xFF);

			// Bring back to the range 0-255
			blendedRed >>= 8;
			blendedGreen >>= 8;
			blendedBlue >>= 8;

			if (blendedAlpha > 0xFF) blendedAlpha = 0xFF;
			if (blendedRed > 0xFF) blendedRed = 0xFF;
			if (blendedGreen > 0xFF) blendedGreen = 0xFF;
			if (blendedBlue > 0xFF) blendedBlue = 0xFF;

			// Put ARGB components back together again
			*destPixels = (blendedAlpha << 24) | (blendedRed << 16) | (blendedGreen << 8) | blendedBlue;

			return true;
		}
	};

	class MultiplyBlendPolicy
	{
	public:

		// Standard multipy blend using an unmodified srcAlpha buffer (the original canvas buffer): dest* invSrcAlpha + (src * dest) * srcAlpha
		static inline void BlendSkip(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t*)
		{
			// Transparent pixels still need to be multiplied, so we have only one route
			Blend(srcPixels, destPixels, globalMultiply);
			srcPixels++, destPixels++;
		}

		// Standard additive blending, with a global alpha multiply. This is the most common requirement for particle effects.
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t*)
		{
			// Transparent pixels still need to be multiplied, so we have only one route
			Blend(srcPixels, destPixels, { 1.0f, 1.0f, 1.0f, 1.0f });
			srcPixels++, destPixels++;
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs an additive blending operation: (src * srcAlpha)+(dest * destAlpha)
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha unmodified
		// *******************************************************************************************************************************************************
		static inline void Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels < 0x00FFFFFF) return; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Source pixels
			uint32_t srcAlpha = src >> 24;
			uint32_t srcRed = (src >> 16) & 0xFF;
			uint32_t srcGreen = (src >> 8) & 0xFF;
			uint32_t srcBlue = src & 0xFF;

			// Destination pixels
			uint32_t destAlpha = dest >> 24;
			uint32_t destRed = (dest >> 16) & 0xFF;
			uint32_t destGreen = (dest >> 8) & 0xFF;
			uint32_t destBlue = dest & 0xFF;

			// Apply a multiplicative blend [ dest*invSrcAlpha + (src*dest)*srcAlpha ]
			uint32_t blendAlpha = static_cast<int>(srcAlpha * globalMultiply.alpha);
			uint32_t invBlendAlpha = (0xFF - blendAlpha) * 0xFF;
			uint32_t blendRed = (uint32_t)(globalMultiply.red * (destRed * invBlendAlpha) + ((srcRed * destRed) * blendAlpha));
			uint32_t blendGreen = (uint32_t)(globalMultiply.green * (destGreen * invBlendAlpha) + ((srcGreen * destGreen) * blendAlpha));
			uint32_t blendBlue = (uint32_t)(globalMultiply.blue * (destBlue * invBlendAlpha) + ((srcBlue * destBlue) * blendAlpha));

			// Bring back to the range 0-255
			blendRed >>= 16;
			blendGreen >>= 16;
			blendBlue >>= 16;

			// Shouldn't be necessary
			if (blendRed > 0xFF) blendRed = 0xFF;
			if (blendGreen > 0xFF) blendGreen = 0xFF;
			if (blendBlue > 0xFF) blendBlue = 0xFF;

			// Put ARGB components back together again
			*destPixels = (destAlpha << 24) | (blendRed << 16) | (blendGreen << 8) | blendBlue;
		}
	};
}
#endif