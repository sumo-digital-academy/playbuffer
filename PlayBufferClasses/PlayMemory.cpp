//********************************************************************************************************************************
//* File:			PlayMemory.cpp
//* Platform:		Independent
//* Description:	Implementation of a simple memory tracker to prevent leaks. Uses #define new which looks neat and tidy
//*                 (especially for new C++ programmers), but doesn't work for placement new, so you are likely to get compile 
//*                 errors if you start including it in your own header files.  
//*                 Avoids use of STL or anything else which allocates memory as this could create infinite loops!
//* Notes:          This is a simple approach to support your understanding of overriding the new operator.
//*                 See below for more advanced approaches:
//*                 1) The CRT Debug Heap Library (a more advanced version of this approach)
//*                 https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2019
//*                 2) Heap Profiling in the debugger 
//*                 https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage?view=vs-2019
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

#ifdef _DEBUG

// Undefine 'new' in this compilation unit only.
#pragma push_macro("new")
#undef new

constexpr int MAX_ALLOCATIONS = 8192 * 4;
constexpr int MAX_FILENAME = 1024;

unsigned int g_allocId = 0;

// A structure to store data on each memory allocation
struct ALLOC
{
	void* address = 0;
	char file[MAX_FILENAME] = { 0 };
	int line = 0;
	size_t size = 0;
	int id = 0;

	ALLOC( void* a, const char* fn, int l, size_t s ) { address = a; line = l; size = s; id = g_allocId++; strcpy_s( file, fn ); };
	ALLOC( void ) {};
};

ALLOC g_allocations[MAX_ALLOCATIONS];
unsigned int g_allocCount = 0;


void CreateStaticObject( void );
void PrintAllocation( const char* tagText, ALLOC& a );

//********************************************************************************************************************************
// Overrides for new operator (x4)
//********************************************************************************************************************************

// The file and line are passed through using the macro defined in PlayMemory.h which redefines new. This will only happen if 
// PlayMemory.h has been parsed in advance of the use of new in the relevant code. This approach is problematic for classes 
// which override the new operator, so putting #include "PlayMemory.h" in relevant cpp files AFTER all other includes is probably
// the safest approach. The two definitions of new without the file and line pick up any other memory allocations for completeness.
void* operator new( size_t size, const char* file, int line )
{
	PLAY_ASSERT( g_allocCount < MAX_ALLOCATIONS );
	CreateStaticObject();
	void* p = malloc( size );
	g_allocations[g_allocCount++] = ALLOC{ p, file, line, size };
	return p;
}

void* operator new[]( size_t size, const char* file, int line )
{
	PLAY_ASSERT( g_allocCount < MAX_ALLOCATIONS );
	CreateStaticObject();
	void* p = malloc( size );
	g_allocations[g_allocCount++] = ALLOC{ p, file, line, size };
	return p;
}

void* operator new( size_t size )
{
	PLAY_ASSERT( g_allocCount < MAX_ALLOCATIONS );
	CreateStaticObject();
	void* p = malloc( size );
	g_allocations[g_allocCount++] = ALLOC{ p, "Unknown", 0, size };
	return p;
}

void* operator new[]( size_t size )
{
	PLAY_ASSERT( g_allocCount < MAX_ALLOCATIONS );
	CreateStaticObject();
	void* p = malloc( size );
	g_allocations[g_allocCount++] = ALLOC{ p, "Unknown", 0, size };
	return p;
}

//********************************************************************************************************************************
// Overrides for delete operator (x4)
//********************************************************************************************************************************

// The definitions with file and line are only included for exception handling, where it looks for a form of delete that matches 
// the new that was used to allocate it.
void operator delete( void* p, const char* file, int line )
{
	UNREFERENCED_PARAMETER( line );
	UNREFERENCED_PARAMETER( file );
	operator delete( p );
}

int g_id = -1;

void operator delete( void* p )
{
	for( unsigned int a = 0; a < g_allocCount; a++ )
	{
		if( g_allocations[a].address == p )
		{
			if( g_allocations[a].id == g_id )
				g_allocations[a].id = g_id;

			g_allocations[a] = g_allocations[g_allocCount - 1];
			g_allocations[g_allocCount - 1].address = nullptr;
			g_allocCount--;
		}
	}
	free( p );
}

void operator delete[]( void* p, const char* file, int line )
{
	UNREFERENCED_PARAMETER( line );
	UNREFERENCED_PARAMETER( file );
	operator delete[]( p );
}

void operator delete[]( void* p )
{
	for( unsigned int a = 0; a < g_allocCount; a++ )
	{
		if( g_allocations[a].address == p )
		{
			if( g_allocations[a].id == g_id )
				g_allocations[a].id = g_id;

			g_allocations[a] = g_allocations[g_allocCount - 1];
			g_allocations[g_allocCount - 1].address = nullptr;
			g_allocCount--;
		}
	}
	free( p );
}

//********************************************************************************************************************************
// Printing allocations
//********************************************************************************************************************************

// A method for printing out all the memory allocation immediately before program exit (or as close as you can get)
// This is achieved by creating a class as a static object before the first memory allocation, which stays in scope until
// after everything else is destroyed. When it is cleaned up its destructor prints the allocations to show us any leaks!
class DestroyedLast
{
public:
	DestroyedLast()
	{
		PrintAllocations( "<STARTUP>" );
	}
	~DestroyedLast()
	{
		if( g_allocCount > 0 )
		{
			PrintAllocations( "<MEMORY LEAK>" );
		}
		else
		{
			DebugOutput( "**************************************************\n" );
			DebugOutput( "NO MEMORY LEAKS!\n" );
			DebugOutput( "**************************************************\n" );
		}

	}
};

void CreateStaticObject( void )
{
	static DestroyedLast last;
}

void PrintAllocation( const char* tagText, ALLOC& a )
{
	char buffer[MAX_FILENAME * 2] = { 0 };

	if( a.address != nullptr )
	{
		char* lastSlash = strrchr( a.file, '\\' );
		if( lastSlash )
		{
			strcpy_s( buffer, lastSlash + 1 );
			strcpy_s( a.file, buffer );
		}
		// Format in such a way that VS can double click to jump to the allocation.
		sprintf_s( buffer, "%s %s(%d): 0x%02X %d bytes [%d]\n", tagText, a.file, a.line, static_cast<int>( reinterpret_cast<long long>( a.address ) ), static_cast<int>( a.size ), a.id );
		DebugOutput( buffer );
	}
}

void PrintAllocations( const char* tagText )
{
	int bytes = 0;
	char buffer[MAX_FILENAME * 2] = { 0 };
	DebugOutput( "****************************************************\n" );
	DebugOutput( "MEMORY ALLOCATED\n" );
	DebugOutput( "****************************************************\n" );
	for( unsigned int n = 0; n < g_allocCount; n++ )
	{
		ALLOC& a = g_allocations[n];
		PrintAllocation( tagText, a );
		bytes += static_cast<int>(a.size);
	}
	sprintf_s( buffer, "%s Total = %d bytes\n", tagText, bytes );
	DebugOutput( buffer );
	DebugOutput( "**************************************************\n" );

}

#pragma pop_macro("new")

#endif