#include "../tomb5/pch.h"
#include "init.h"
#include "../global/types.h"

void* game_malloc(int size)
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

void inject_init()
{
	INJECT(0x004A7D00, game_malloc);
}