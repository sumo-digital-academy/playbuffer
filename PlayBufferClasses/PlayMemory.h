#ifndef PLAY_PLAYMEMORY_H
#define PLAY_PLAYMEMORY_H
//********************************************************************************************************************************
// File:		PlayMemory.h
// Platform:	Independent
// Description:	Declaration for a simple memory tracker to prevent leaks
//********************************************************************************************************************************

#ifdef _DEBUG
	// Prints out all the currently allocated memory to the debug output
	void PrintAllocations( const char* tagText );

	// Allocate some memory with a known origin
	void* operator new(size_t size, const char* file, int line);
	// Allocate some memory with a known origin
	void* operator new[](size_t size, const char* file, int line); 
	// Allocate some memory without a known origin
	void* operator new[](size_t size);

	// Free some memory 
	void operator delete[](void* p);
	// Free some memory (matching allocator for exceptions )
	void operator delete(void* p, const char* file, int line);
	// Free some memory (matching allocator for exceptions )
	void operator delete[](void* p, const char* file, int line); 

	#define new new( __FILE__ , __LINE__ )
#else
	#define PrintAllocations( x )
#endif

#endif