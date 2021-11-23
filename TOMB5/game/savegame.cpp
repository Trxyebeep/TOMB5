#include "../tomb5/pch.h"
#include "savegame.h"

void WriteSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
	{
		*SGpoint = *data;
		data++;
		SGpoint++;
	}
}

void ReadSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
	{
		*data = *SGpoint;
		data++;
		SGpoint++;
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x00470EC0, WriteSG, replace);
	INJECT(0x00470F10, ReadSG, replace);
}
