#include "../tomb5/pch.h"
#include "file.h"
#include "function_stubs.h"
#include "../game/gameflow.h"
#include "../game/setup.h"
#include "others.h"

//when every part that uses the c library funcs is decompiled, remove the stupid defines

bool LoadTextureInfos()
{
	int val;
	PHDTEXTURESTRUCT tex;

	Log(2, "LoadTextureInfos");
	FileData += sizeof(long);
	val = *(long*)FileData;
	FileData += sizeof(long);
	Log(5, "Texture Infos : %d", val);
	textinfo = (TEXTURESTRUCT*)game_malloc(val * sizeof(TEXTURESTRUCT), 0);

	for (int i = 0; i < val; i++)
	{
		memcpy(&tex, FileData, sizeof(PHDTEXTURESTRUCT));
		FileData += sizeof(PHDTEXTURESTRUCT);
		textinfo[i].drawtype = tex.drawtype;
		textinfo[i].tpage = tex.tpage & 0x7FFF;
		textinfo[i].flag = tex.tpage ^ (tex.tpage ^ tex.flag) & 0x7FFF;

#ifdef fix_that_ugly_nose
		if (
			(gfCurrentLevel == LVL5_STREETS_OF_ROME && (i == 200 || i == 204)) ||
			(gfCurrentLevel == LVL5_TRAJAN_MARKETS && (i == 225 || i == 229)) ||
			(gfCurrentLevel == LVL5_COLOSSEUM && (i == 244 || i == 240)) ||
			(gfCurrentLevel == LVL5_BASE && (i == 213 || i == 210)) ||
			(gfCurrentLevel == LVL5_SUBMARINE && (i == 205 || i == 208)) ||
			(gfCurrentLevel == LVL5_DEEPSEA_DIVE && (i == 201 || i == 205)) ||
			(gfCurrentLevel == LVL5_SINKING_SUBMARINE && (i == 238 || i == 235)) ||
			((gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL) && (i == 148 || i == 144)) ||
			(gfCurrentLevel == LVL5_THIRTEENTH_FLOOR && (i == 99 || i == 103)) ||
			(gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS && (i == 105 || i == 101)) ||
			(gfCurrentLevel == LVL5_RED_ALERT && (i == 133 || i == 137))
			)
		{
			textinfo[i].u1 = (tex.u1 >> 8) * 0.00390625f;
			textinfo[i].v1 = (tex.v1 >> 8) * 0.00390625f;

			textinfo[i].u2 = (tex.u2 >> 8) * 0.00393690f;
			textinfo[i].v2 = (tex.v2 >> 8) * 0.00393690f;
			textinfo[i].u3 = (tex.u3 >> 8) * 0.00393690f;
			textinfo[i].v3 = (tex.v3 >> 8) * 0.00393690f;

			textinfo[i].u4 = (tex.u4 >> 8) * 0.00390625f;
			textinfo[i].v4 = (tex.v4 >> 8) * 0.00390625f;
			continue;
		}
#endif
		textinfo[i].u1 = (tex.u1 >> 8) * 0.00390625f;
		textinfo[i].v1 = (tex.v1 >> 8) * 0.00390625f;
		textinfo[i].u2 = (tex.u2 >> 8) * 0.00390625f;
		textinfo[i].v2 = (tex.v2 >> 8) * 0.00390625f;
		textinfo[i].u3 = (tex.u3 >> 8) * 0.00390625f;
		textinfo[i].v3 = (tex.v3 >> 8) * 0.00390625f;
		textinfo[i].u4 = (tex.u4 >> 8) * 0.00390625f;
		textinfo[i].v4 = (tex.v4 >> 8) * 0.00390625f;
	}

	AdjustUV(val);
	Log(5, "Created %d Texture Pages", nTextures - 1);
	return 1;
}

bool LoadRooms()
{
	long size;

	Log(2, "LoadRooms");
	wibble = 0;
	MaxRoomLights = 0;
	LevelFogBulbCnt = 0;
	aLoadRoomStream();
	BuildOutsideTable();
	size = *(long*)FileData;
	FileData += sizeof(long);
	floor_data = (short*)game_malloc(size * sizeof(short), 0);
	memcpy(floor_data, FileData, sizeof(short) * size);
	FileData += sizeof(short) * size;
	Log(0, "Floor Data Size %d @ %x", size, floor_data);
	return 1;
}

FILE* FileOpen(const char* Filename)
{
	FILE* fp;
	char cdFilename[80];

	memset(cdFilename, 0, 80);
#ifndef NO_CD
	cdFilename[0] = cd_drive;
	cdFilename[1] = ':';//original code
	cdFilename[2] = '\\';
#endif

	strcat(cdFilename, Filename);
	Log(5, "FileOpen - %s", cdFilename);
#define what_the_f	( (FILE*(__cdecl*)(const char*, const char*)) 0x004E46E0 )//temporary until we solve the fopen mystery :)
	fp = what_the_f(cdFilename, "rb");//fp = fopen(cdFilename, "rb");
#undef what_the_f

	if (!fp)
		Log(1, "Unable To Open %s", cdFilename);

	return fp;
}

bool FindCDDrive()
{
	unsigned long drives;
	unsigned long type;
	char root[5];
	char file_check[14];
	HANDLE file;

	strcpy(file_check, "c:\\script.dat");
	drives = GetLogicalDrives();
	cd_drive = 'A';
	lstrcpy(root, "A:\\");

	while (drives)
	{
		if (drives & 1)
		{
			root[0] = cd_drive;
			type = GetDriveType(root);

			if (type == DRIVE_CDROM)
			{
				file_check[0] = cd_drive;
				file = CreateFile(file_check, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (file != INVALID_HANDLE_VALUE)
				{
					CloseHandle(file);
					return 1;
				}
			}
		}

		cd_drive++;
		drives >>= 1;
	}

	return 0;
}

void FileClose(FILE* fp)
{
	Log(2, "FileClose");
#define what_the_f	( (int(__cdecl*)(FILE*)) 0x004E20D0 )
	what_the_f(fp);//fclose(fp);
#undef what_the_f
}

int FileSize(FILE* fp)
{
	int size;
#define cunt1	( (int(__cdecl*)(FILE*, long, int)) 0x004E1F30 )
#define cunt2	( (int(__cdecl*)(FILE*)) 0x004E4700 )
	cunt1(fp, 0, SEEK_END);//fseek(fp, 0, SEEK_END);
	size = cunt2(fp);//ftell(fp);
	cunt1(fp, 0, SEEK_SET);//fseek(fp, 0, SEEK_SET);
	return size;
#undef cunt1
#undef cunt2
}

void inject_file()
{
	INJECT(0x004A60E0, LoadTextureInfos);
	INJECT(0x004A4DA0, LoadRooms);
	INJECT(0x004A3CD0, FileOpen);
	INJECT(0x004A3BC0, FindCDDrive);
	INJECT(0x004A3DA0, FileClose);
	INJECT(0x004A3DD0, FileSize);
}
