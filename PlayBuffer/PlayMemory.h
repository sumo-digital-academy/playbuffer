#ifndef PLAY_PLAYMEMORY_H
#define PLAY_PLAYMEMORY_H
//********************************************************************************************************************************
// File:		PlayMemory.h
// Platform:	Independent
// Description:	Declaration for a simple memory tracker to prevent leaks
//********************************************************************************************************************************
#ifdef _DEBUG
	// Prints out all the currently allocated memory to the debug output
namespace Play
{
	void PrintAllocations(const char* tagText);
}; // namespace Play
	void* operator new  (std::size_t sizeBytes);
	void* operator new[](std::size_t sizeBytes);

	void operator delete  (void* ptr) noexcept;
	void operator delete[](void* ptr) noexcept;
#else
namespace Play
{ 
	inline void PrintAllocations(const char*) {};
} // namespace Play
#endif

#endif // PLAY_PLAYMEMORY_H