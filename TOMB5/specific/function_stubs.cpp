#include "../tomb5/pch.h"
#include "function_stubs.h"

FILE* logF = 0;

char* malloc_buffer;
char* malloc_ptr;
long malloc_size;
long malloc_free;

long nPolyType;

static long malloc_used;

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

void* game_malloc(long size, long type)
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
		memset(ptr, 0, size);
		return ptr;
	}
}

long GetRandomControl()
{
	rand_1 = 1103515245 * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
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

void deadLog(const char* string, ...)
{

}

void DBG_Print(const char* string, ...)
{
	va_list list;
	char buf[4096];

	va_start(list, string);
	vsprintf(buf, string, list);
	va_end(list);
	Log(0, buf);
}

void exit_message()
{

}

void S_ExitSystem(const char* string)
{
	Log(0, "**** %s ****", string);
}

void S_InitialiseScreen()
{

}

void S_CalculateStaticLight()
{

}

void S_CalculateLight(long x, long y, long z, short room_number, ITEM_LIGHT* il)
{

}

void S_SetReverbType(short type)
{

}

void S_CDVolume(long vol)
{

}

void S_CDLoop()
{

}

void PrintDbug()
{

}

void init_game_malloc()
{
	malloc_buffer = (char*)malloc(MALLOC_SIZE);
	malloc_size = MALLOC_SIZE;
	malloc_ptr = malloc_buffer;
	malloc_free = MALLOC_SIZE;
	malloc_used = 0;
}

void game_free(long size)
{
	size = (size + 3) & 0xFC;
	malloc_ptr -= size;
	malloc_free += size;
	malloc_used -= size;
}

void Log(ulong type, const char* s, ...)
{
#ifdef DO_LOG
	va_list list;
	char buf[4096];

	if (!logF)
		logF = fopen("log.txt", "w+");

	va_start(list, s);
	vsprintf(buf, s, list);
	strcat(buf, "\n");
	va_end(list);
	fwrite(buf, strlen(buf), 1, logF);
#endif
}
