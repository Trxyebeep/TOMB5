#include "../tomb5/pch.h"
#include "function_stubs.h"

void* game_malloc(int size, int type)
{
	char* ptr;

	size = (size + 3) & -4;

	if (size > malloc_free)
	{
		Log(0, "OUT OF MEMORY");
		return 0;
	}
	else
	{
		ptr = malloc_ptr;
		malloc_free -= size;
		malloc_used += size;
		malloc_ptr += size;
		memset(ptr, 0, 4 * ((unsigned int)size >> 2));
		return ptr;
	}
}

int GetRandomControl()
{
	rand_1 = 1103515245 * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

int GetRandomDraw()
{
	rand_2 = 1103515245 * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

void S_SoundSetMasterVolume()
{

}

void inject_funcStubs(bool replace)
{
	INJECT(0x004A7D00, game_malloc, replace);
	INJECT(0x004A7C10, GetRandomControl, replace);
	INJECT(0x004A7C70, SeedRandomControl, replace);
	INJECT(0x004A7C40, GetRandomDraw, replace);
	INJECT(0x004A7C90, SeedRandomDraw, replace);
	INJECT(0x004A7B30, S_SoundSetMasterVolume, replace);
}
