//********************************************************************************************************************************
//* File:			PlayMemory.cpp
//* Platform:		Independent
//* Description:	Implementation of a simple memory tracker to prevent leaks. 
//*                 Avoids use of STL or anything else which allocates memory as this could create infinite loops!
//* Notes:          This is a simple approach to support your understanding of overriding the new operator.
//*                 See below for alternative approaches:
//*                 1) The CRT Debug Heap Library 
//*                 https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2019
//*                 2) Heap Profiling in the debugger 
//*                 https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage?view=vs-2019
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

#pragma comment(lib, "DbgHelp.lib")

#ifdef _DEBUG

namespace Play
{
	constexpr int MAX_ALLOCATIONS = 8192 * 4;
	constexpr int MAX_FILENAME = 1024;
	constexpr int STACKTRACE_OFFSET = 2;
	constexpr int STACKTRACE_DEPTH = 1;

	static unsigned int g_allocId = 0;

	// A structure to store data on each memory allocation
	struct ALLOC
	{
		void* address = 0;
		size_t sizeBytes = 0;
		int id = 0;
		void* stack[STACKTRACE_DEPTH];
		int frames;
	};

	ALLOC g_allocations[MAX_ALLOCATIONS];
	unsigned int g_allocCount = 0;
	int g_id = -1;
	void* g_allocDeleted = nullptr;

	CRITICAL_SECTION g_allocCritSec;

	// A method for printing out all the memory allocation immediately before program exit (or as close as you can get)
	// This is achieved by creating a class as a static object before the first memory allocation, which stays in scope until
	// after everything else is destroyed. When it is cleaned up its destructor prints the allocations to show us any leaks!
	class DestroyedLast
	{
	public:
		DestroyedLast()
		{
			InitializeCriticalSectionAndSpinCount(&g_allocCritSec, 64);

#ifdef _DEBUG
			HANDLE process = GetCurrentProcess();
			SymInitialize(process, NULL, TRUE);
			SymSetOptions(SYMOPT_LOAD_LINES);
#endif

			PrintAllocations("<STARTUP>");
		}
		~DestroyedLast()
		{
			if (g_allocCount > 0)
			{
				PrintAllocations("<MEMORY LEAK>");
			}
			else
			{
				DebugOutput("**************************************************\n");
				DebugOutput("NO MEMORY LEAKS!\n");
				DebugOutput("**************************************************\n");
			}

			DeleteCriticalSection(&g_allocCritSec);
		}
	};

	void CreateStaticObject(void)
	{
		static DestroyedLast last;
	}

	void TrackAllocation(void* ptr, std::size_t sizeBytes)
	{
		CreateStaticObject();

		EnterCriticalSection(&g_allocCritSec);

		PLAY_ASSERT(g_allocCount < MAX_ALLOCATIONS);
		ALLOC& rAlloc(g_allocations[g_allocCount++]);
		rAlloc.address = ptr;
		rAlloc.sizeBytes = sizeBytes;
		rAlloc.frames = RtlCaptureStackBackTrace(STACKTRACE_OFFSET, STACKTRACE_DEPTH, rAlloc.stack, NULL);

		LeaveCriticalSection(&g_allocCritSec);
	}

	void UntrackAllocation(void* ptr)
	{
		EnterCriticalSection(&g_allocCritSec);
		g_allocDeleted = ptr;

		for (unsigned int i = 0; i < g_allocCount; i++)
		{
			if (g_allocations[i].address == ptr)
			{
				if (g_allocations[i].id == g_id)
					g_allocations[i].id = g_id;

				g_allocations[i] = g_allocations[g_allocCount - 1];
				g_allocations[g_allocCount - 1].address = nullptr;
				g_allocCount--;

				// Early out
				LeaveCriticalSection(&g_allocCritSec);
				return;
			}
		}
		LeaveCriticalSection(&g_allocCritSec);
	}

	//********************************************************************************************************************************
	// Printing allocations
	//********************************************************************************************************************************

	void PrintAllocation(const char* tagText, const ALLOC& rAlloc)
	{
		char buffer[MAX_FILENAME * 2] = { 0 };

		if (rAlloc.address != nullptr)
		{
			HANDLE process = GetCurrentProcess();

			DWORD  dwDisplacement;

			IMAGEHLP_LINE64 line;
			constexpr size_t MAX_SYM_NAME_LENGTH = 255;
			SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME_LENGTH + 1);
			symbol->MaxNameLen = MAX_SYM_NAME_LENGTH;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			sprintf_s(buffer, "%s 0x%02p %d bytes [%d]: \n", tagText, rAlloc.address, static_cast<int>(rAlloc.sizeBytes), rAlloc.id);
			DebugOutput(buffer);
			for (int i = 0; i < rAlloc.frames; ++i)
			{
				SymFromAddr(process, (DWORD64)(rAlloc.stack[i]), 0, symbol);
				SymGetLineFromAddr64(process, (DWORD64)rAlloc.stack[i], &dwDisplacement, &line);

				// Format in such a way that VS can double click to jump to the allocation.
				sprintf_s(buffer, "%s(%d): %s\n", line.FileName, line.LineNumber, symbol->Name);

				DebugOutput(buffer);
			}

			free(symbol);
		}
	}

	void PrintAllocations(const char* tagText)
	{
		EnterCriticalSection(&g_allocCritSec);

		int bytes = 0;
		char buffer[MAX_FILENAME * 2] = { 0 };
		DebugOutput("****************************************************\n");
		DebugOutput("MEMORY ALLOCATED\n");
		DebugOutput("****************************************************\n");
		for (unsigned int n = 0; n < g_allocCount; n++)
		{
			ALLOC& a = g_allocations[n];
			PrintAllocation(tagText, a);
			bytes += static_cast<int>(a.sizeBytes);
		}
		sprintf_s(buffer, "%s Total = %d bytes\n", tagText, bytes);
		DebugOutput(buffer);
		DebugOutput("**************************************************\n");

		LeaveCriticalSection(&g_allocCritSec);
	}

} // namespace Play

//********************************************************************************************************************************
// Overrides for new operator (x2)
//********************************************************************************************************************************
void* operator new  (std::size_t sizeBytes)
{
	void* ptr = malloc(sizeBytes);
	Play::TrackAllocation(ptr, sizeBytes);
	return ptr;
}

void* operator new[](std::size_t sizeBytes)
{
	void* ptr = malloc(sizeBytes);
	Play::TrackAllocation(ptr, sizeBytes);
	return ptr;
}

//********************************************************************************************************************************
// Overrides for delete operator (x2)
//********************************************************************************************************************************
void operator delete  (void* ptr) noexcept
{
	Play::UntrackAllocation(ptr);
	free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	Play::UntrackAllocation(ptr);
	free(ptr);
}
#endif