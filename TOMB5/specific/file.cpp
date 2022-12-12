#include "../tomb5/pch.h"
#include "file.h"
#include "function_stubs.h"
#include "../game/gameflow.h"
#include "../game/setup.h"
#include "alexstuff.h"
#include "../game/init.h"
#include "../game/items.h"
#include "../game/objects.h"
#include "../game/laraskin.h"
#include "drawroom.h"
#include "dxsound.h"
#include "LoadSave.h"
#include "function_table.h"
#include "polyinsert.h"
#include "winmain.h"
#include "output.h"
#ifdef GENERAL_FIXES
#include "../tomb5/tomb5.h"
#endif
#include "texture.h"
#include "dxshell.h"

bool LoadTextureInfos()
{
	long val;
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

#ifdef GENERAL_FIXES
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
			textinfo[i].u1 = (tex.u1 >> 8) * (1.0f / 256.0f);
			textinfo[i].v1 = (tex.v1 >> 8) * (1.0f / 256.0f);

			textinfo[i].u2 = (tex.u2 >> 8) * 0.00393690f;
			textinfo[i].v2 = (tex.v2 >> 8) * 0.00393690f;
			textinfo[i].u3 = (tex.u3 >> 8) * 0.00393690f;
			textinfo[i].v3 = (tex.v3 >> 8) * 0.00393690f;

			textinfo[i].u4 = (tex.u4 >> 8) * (1.0f / 256.0f);
			textinfo[i].v4 = (tex.v4 >> 8) * (1.0f / 256.0f);
			continue;
		}
#endif
		textinfo[i].u1 = (tex.u1 >> 8) * (1.0f / 256.0f);
		textinfo[i].v1 = (tex.v1 >> 8) * (1.0f / 256.0f);
		textinfo[i].u2 = (tex.u2 >> 8) * (1.0f / 256.0f);
		textinfo[i].v2 = (tex.v2 >> 8) * (1.0f / 256.0f);
		textinfo[i].u3 = (tex.u3 >> 8) * (1.0f / 256.0f);
		textinfo[i].v3 = (tex.v3 >> 8) * (1.0f / 256.0f);
		textinfo[i].u4 = (tex.u4 >> 8) * (1.0f / 256.0f);
		textinfo[i].v4 = (tex.v4 >> 8) * (1.0f / 256.0f);
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
	cdFilename[1] = ':';
	cdFilename[2] = '\\';
#endif

	strcat(cdFilename, Filename);
	Log(5, "FileOpen - %s", cdFilename);
	fp = OPEN(cdFilename, "rb");

	if (!fp)
		Log(1, "Unable To Open %s", cdFilename);

	return fp;
}

bool FindCDDrive()
{
	ulong drives;
	ulong type;
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
	CLOSE(fp);
}

long FileSize(FILE* fp)
{
	long size;

	SEEK(fp, 0, SEEK_END);
	size = TELL(fp);
	SEEK(fp, 0, SEEK_SET);
	return size;
}

bool LoadItems()
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	STATIC_INFO* stat;
	long x, y, z;

	Log(2, "LoadItems");
	num_items = *(long*)FileData;
	FileData += 4;

	if (!num_items)
		return 1;

	items = (ITEM_INFO*)game_malloc(MAX_ITEMS * sizeof(ITEM_INFO), 0);
	level_items = num_items;
	InitialiseClosedDoors();
	InitialiseItemArray(MAX_ITEMS);

	for (int i = 0; i < num_items; i++)
	{
		item = &items[i];
		item->object_number = *(short*)FileData;
		FileData += 2;
		item->room_number = *(short*)FileData;
		FileData += 2;
		item->pos.x_pos = *(long*)FileData;
		FileData += 4;
		item->pos.y_pos = *(long*)FileData;
		FileData += 4;
		item->pos.z_pos = *(long*)FileData;
		FileData += 4;
		item->pos.y_rot = *(short*)FileData;
		FileData += 2;
		item->shade = *(short*)FileData;
		FileData += 2;
		item->trigger_flags = *(short*)FileData;
		FileData += 2;
		item->flags = *(short*)FileData;
		FileData += 2;
	}

	for (int i = 0; i < num_items; i++)
		InitialiseItem(i);

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (int j = 0; j < r->num_meshes; j++)
		{
			x = (r->mesh[j].x - r->x) >> 10;
			z = (r->mesh[j].z - r->z) >> 10;

			floor = &(r->floor[x * r->x_size + z]);

			if (!(boxes[floor->box].overlap_index & 0x4000) && (gfCurrentLevel != 4 || i != 19 && i != 23 && i != 16))
			{
				stat = &static_objects[r->mesh[j].static_number];
				y = floor->floor << 8;

				if (y <= (r->mesh[j].y - stat->y_maxc + 512) && y < r->mesh[j].y - stat->y_minc)
				{
					if (!stat->x_maxc || !stat->x_minc || !stat->z_maxc || !stat->z_minc ||
						(stat->x_maxc ^ stat->x_minc) & 0x8000 && (stat->z_maxc ^ stat->z_minc) & 0x8000)
					{
						x = (r->mesh[j].x - r->x) >> 10;
						z = (r->mesh[j].z - r->z) >> 10;
						r->floor[x * r->x_size + z].stopper = 1;
					}
				}
			}
		}
	}

	return 1;
}

bool LoadSprites()
{
	STATIC_INFO* stat;
	OBJECT_INFO* obj;
	PHDSPRITESTRUCT sprite;
	long num_sprites, num_slots, slot;

	Log(2, "LoadSprites");
	FileData += 4;
	num_sprites = *(long*)FileData;
	FileData += 4;
	spriteinfo = (SPRITESTRUCT*)game_malloc(sizeof(SPRITESTRUCT) * num_sprites, 0);

	for (int i = 0; i < num_sprites; i++)
	{
		memcpy(&sprite, FileData, sizeof(PHDSPRITESTRUCT));
		FileData += sizeof(PHDSPRITESTRUCT);
		spriteinfo[i].height = sprite.height;
		spriteinfo[i].offset = sprite.offset;
		spriteinfo[i].tpage = sprite.tpage;
		spriteinfo[i].width = sprite.width;
		spriteinfo[i].x1 = (float)(sprite.x1) * (1.0F / 256.0F);
		spriteinfo[i].y1 = (float)(sprite.y1) * (1.0F / 256.0F);
		spriteinfo[i].x2 = (float)(sprite.x2) * (1.0F / 256.0F);
		spriteinfo[i].y2 = (float)(sprite.y2) * (1.0F / 256.0F);
		spriteinfo[i].x1 += (1.0F / 256.0F);
		spriteinfo[i].y1 += (1.0F / 256.0F);
		spriteinfo[i].x2 -= (1.0F / 256.0F);
		spriteinfo[i].y2 -= (1.0F / 256.0F);
		spriteinfo[i].tpage++;
	}

	num_slots = *(long*)FileData;
	FileData += 4;

	if (num_slots <= 0)
		return 1;

	for (int i = 0; i < num_slots; i++)
	{
		slot = *(long*)FileData;
		FileData += 4;

		if (slot >= NUMBER_OBJECTS)
		{
			slot -= NUMBER_OBJECTS;
			stat = &static_objects[slot];
			stat->mesh_number = *(short*)FileData;
			FileData += 2;
			stat->mesh_number = *(short*)FileData;
			FileData += 2;
		}
		else
		{
			obj = &objects[slot];
			obj->nmeshes = *(short*)FileData;
			FileData += 2;
			obj->mesh_index = *(short*)FileData;
			FileData += 2;
			obj->loaded = 1;
		}
	}

	return 1;
}

bool LoadCameras()
{
	Log(2, "LoadCameras");
	number_cameras = *(long*)FileData;
	FileData += 4;

	if (number_cameras)
	{
		camera.fixed = (OBJECT_VECTOR*)game_malloc(number_cameras * sizeof(OBJECT_VECTOR), 0);
		memcpy(camera.fixed, FileData, number_cameras * sizeof(OBJECT_VECTOR));
		FileData += number_cameras * sizeof(OBJECT_VECTOR);
	}

	number_spotcams = *(short*)FileData;	//it's a short to avoid warning but it reserves 4 bytes in the level file
	FileData += 4;							//so +4, not +2

	if (number_spotcams)
	{
		memcpy(SpotCam, FileData, number_spotcams * sizeof(SPOTCAM));
		FileData += number_spotcams * sizeof(SPOTCAM);
	}

	return 1;
}

bool LoadSoundEffects()
{
	Log(2, "LoadSoundEffects");
	number_sound_effects = *(long*)FileData;
	FileData += 4;
	Log(8, "Number of SFX %d", number_sound_effects);

	if (number_sound_effects)
	{
		sound_effects = (OBJECT_VECTOR*)game_malloc(number_sound_effects * sizeof(OBJECT_VECTOR), 0);
		memcpy(sound_effects, FileData, number_sound_effects * sizeof(OBJECT_VECTOR));
		FileData += number_sound_effects * sizeof(OBJECT_VECTOR);
	}

	return 1;
}

bool LoadAnimatedTextures()
{
	long num_anim_ranges;

	num_anim_ranges = *(long*)FileData;
	FileData += 4;
	aranges = (short*)game_malloc(num_anim_ranges * 2, 0);
	memcpy(aranges, FileData, num_anim_ranges * 2);

#ifdef GENERAL_FIXES
	if (gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS) // Fixes UVRotate in lift cutscene
		aranges[2] = 2076;
#endif

	FileData += num_anim_ranges * sizeof(short);
	nAnimUVRanges = *(char*)FileData;
	FileData += 1;
	return 1;
}

bool LoadObjects()
{
	OBJECT_INFO* obj;
	STATIC_INFO* stat;
	short** mesh;
	short** mesh_size;
	long num, slot;
	static long num_meshes;
	static long num_anims;

	Log(2, "LoadObjects");
	memset(objects, 0, NUMBER_OBJECTS * sizeof(OBJECT_INFO));
	memset(static_objects, 0, 70 * sizeof(STATIC_INFO));

	num = *(long*)FileData;	//meshes
	FileData += 4;
	mesh_base = (short*)game_malloc(num * 2, 0);
	memcpy(mesh_base, FileData, num * 2);
	FileData += num * 2;

	num = *(long*)FileData;
	FileData += 4;
	meshes = (short**)game_malloc(num * 8, 0);
	memcpy(meshes, FileData, num * 4);
	FileData += num * 4;

	for (int i = 0; i < num; i++)
		meshes[i] = &mesh_base[(long)meshes[i] / 2];

	num_meshes = num;

	num_anims = *(long*)FileData;	//anims
	FileData += 4;
	anims = (ANIM_STRUCT*)game_malloc(num_anims * sizeof(ANIM_STRUCT), 0);
	memcpy(anims, FileData, num_anims * sizeof(ANIM_STRUCT));
	FileData += num_anims * sizeof(ANIM_STRUCT);

	num = *(long*)FileData;	//changes
	FileData += 4;
	changes = (CHANGE_STRUCT*)game_malloc(num * sizeof(CHANGE_STRUCT), 0);
	memcpy(changes, FileData, num * sizeof(CHANGE_STRUCT));
	FileData += num * sizeof(CHANGE_STRUCT);

	num = *(long*)FileData;	//ranges
	FileData += 4;
	ranges = (RANGE_STRUCT*)game_malloc(num * sizeof(RANGE_STRUCT), 0);
	memcpy(ranges, FileData, num * sizeof(RANGE_STRUCT));
	FileData += num * sizeof(RANGE_STRUCT);

	num = *(long*)FileData;	//anim commands
	FileData += 4;
	commands = (short*)game_malloc(num * 2, 0);
	memcpy(commands, FileData, num * 2);
	FileData += num * 2;

	num = *(long*)FileData;	//bones
	FileData += 4;
	bones = (long*)game_malloc(num * 4, 0);
	memcpy(bones, FileData, num * 4);
	FileData += num * 4;

	num = *(long*)FileData;	//frames
	FileData += 4;
	frames = (short*)game_malloc(num * 2, 0);
	memcpy(frames, FileData, num * 2);
	FileData += num * 2;

	for (int i = 0; i < num_anims; i++)
		anims[i].frame_ptr = (short*)((long)anims[i].frame_ptr + (long)frames);

	num = *(long*)FileData;
	FileData += 4;

	for (int i = 0; i < num; i++)
	{
		slot = *(long*)FileData;
		FileData += 4;
		obj = &objects[slot];

		obj->nmeshes = *(short*)FileData;
		FileData += 2;

		obj->mesh_index = *(short*)FileData;
		FileData += 2;

		obj->bone_index = *(long*)FileData;
		FileData += 4;

		obj->frame_base = (short*)(*(short**)FileData);
		FileData += 4;

		obj->anim_index = *(short*)FileData;
		FileData += 2;

		obj->loaded = 1;
		slot = *(short*)FileData;	//pad?
		FileData += 2;
	}

	if (LaraDrawType != LARA_DIVESUIT)
		CreateSkinningData();

	for (int i = 0; i < NUMBER_OBJECTS; i++)
	{
		obj = &objects[i];
		obj->mesh_index *= 2;
	}

	mesh = meshes;
	mesh_size = &meshes[num_meshes];
	memcpy(mesh_size, mesh, num_meshes * 4);

	for (int i = 0; i < num_meshes; i++)
	{
		*mesh = *mesh_size;
		mesh++;
		*mesh = *mesh_size;
		mesh++;
		mesh_size++;
	}

	InitialiseObjects();
	InitialiseClosedDoors();

	num = *(long*)FileData;	//statics
	FileData += 4;

	for (int i = 0; i < num; i++)
	{
		slot = *(long*)FileData;
		FileData += 4;
		stat = &static_objects[slot];

		stat->mesh_number = *(short*)FileData;
		FileData += 2;

		memcpy(&stat->x_minp, FileData, 12);
		FileData += 12;

		memcpy(&stat->x_minc, FileData, 12);
		FileData += 12;

		stat->flags = *(short*)FileData;
		FileData += 2;
	}

	for (int i = 0; i < 70; i++)
	{
		stat = &static_objects[i];
		stat->mesh_number *= 2;
	}

	ProcessMeshData(num_meshes * 2);
	return 1;
}

bool LoadCinematic()
{
	FileData += 2;
	return 1;
}

bool LoadSamples()
{
	long num_samples, uncomp_size, comp_size;
	static long num_sample_infos;

	Log(2, "LoadSamples");
	sample_lut = (short*)game_malloc(MAX_SAMPLES * sizeof(short), 0);
	memcpy(sample_lut, FileData, MAX_SAMPLES * sizeof(short));
	FileData += MAX_SAMPLES * sizeof(short);
	num_sample_infos = *(long*)FileData;
	FileData += 4;
	Log(8, "Number Of Sample Infos %d", num_sample_infos);

	if (!num_sample_infos)
	{
		Log(1, "No Sample Infos");
		return 0;
	}

	sample_infos = (SAMPLE_INFO*)game_malloc(sizeof(SAMPLE_INFO) * num_sample_infos, 0);
	memcpy(sample_infos, FileData, sizeof(SAMPLE_INFO) * num_sample_infos);
	FileData += sizeof(SAMPLE_INFO) * num_sample_infos;
	num_samples = *(long*)FileData;
	FileData += 4;

	if (!num_samples)
	{
		Log(1, "No Samples");
		return 0;
	}

	Log(8, "Number Of Samples %d", num_samples);
	READ(&num_samples, 1, 4, LevelFILEptr);

	if (feof(LevelFILEptr))
		Log(1, "END OF FILE");

	InitSampleDecompress();

	if (num_samples <= 0)
	{
		FreeSampleDecompress();
		return 1;
	}

	for (int i = 0; i < num_samples; i++)
	{
		READ(&uncomp_size, 1, 4, LevelFILEptr);
		READ(&comp_size, 1, 4, LevelFILEptr);
		READ(samples_buffer, comp_size, 1, LevelFILEptr);

		if (!DXCreateSampleADPCM(samples_buffer, comp_size, uncomp_size, i))
		{
			FreeSampleDecompress();
			return 0;
		}
	}

	FreeSampleDecompress();
	return 1;
}

bool LoadAIInfo()
{
	long num_ai;

	num_ai = *(long*)FileData;
	FileData += 4;

	if (!num_ai)
		return 1;

	nAIObjects = (short)num_ai;
	AIObjects = (AIOBJECT*)game_malloc(sizeof(AIOBJECT) * num_ai, 0);
	memcpy(AIObjects, FileData, sizeof(AIOBJECT) * num_ai);
	FileData += sizeof(AIOBJECT) * num_ai;
	return 1;
}

unsigned int __stdcall LoadLevel(void* name)
{
	return 1;
}

long S_LoadLevelFile(long num)
{
	static long lscreen = 0;
	static long flag = 0;
	long chosen_screen;
	char name[80];

	Log(2, "S_LoadLevelFile");

#ifdef GENERAL_FIXES
	if (!tomb5.tr4_loadscreens || (!num && !bDoCredits && !gfStatus))
#endif
	{
		if (!MonoScreenOn)
		{
			chosen_screen = num;

			if (!num)
			{
				if (flag)
				{
					chosen_screen = lscreen % 3 + 15;
					lscreen++;
				}
				else
				{
					flag = 1;
					chosen_screen = -2;
				}
			}

			LoadScreen(chosen_screen + 2, 4);
		}
	}

	strcpy(name, &gfFilenameWad[gfFilenameOffset[num]]);
	loadbar_on = 0;
	strcat(name, ".TRC");

	for (int i = 0; i < 4; i++)
	{
		_BeginScene();
		InitBuckets();
		InitialiseSortList();
		DrawLoadingScreen();
		SortPolyList(SortCount, SortList);
		RestoreFPCW(FPCW);
		DrawSortList();
		MungeFPCW(&FPCW);
		S_DumpScreenFrame();
	}

#ifdef GENERAL_FIXES
	if (!tomb5.tr4_loadscreens || (!num && !bDoCredits && !gfStatus))
#endif
	{
#ifdef GENERAL_FIXES
		if (MonoScreenOn == 2)
#else
		if (MonoScreenOn == 1)
#endif
			ReleaseScreen();
	}

	LevelLoadingThread.active = 1;
	LevelLoadingThread.ended = 0;
	LevelLoadingThread.handle = _beginthreadex(0, 0, LoadLevel, name, 0, (unsigned int*)&LevelLoadingThread.address);

	while (LevelLoadingThread.active)
	{
		if (App.dx.Flags & 0x80 && loadbar_on)
			S_DrawLoadBar();
	}

	if (App.dx.Flags & 0x80 && !S_DrawLoadBar())
		while (!S_DrawLoadBar());

	return 1;
}

bool LoadTextures(long RTPages, long OTPages, long BTPages)
{
	DXTEXTUREINFO* dxtex;
	LPDIRECTDRAWSURFACE4 tSurf;
	LPDIRECT3DTEXTURE2 pTex;
	uchar* TextureData;
	long* d;
	char* pData;
	char* pComp;
	char* s;
	long format, skip, size, compressedSize, nTex, c;
	uchar r, g, b, a;

	Log(2, "LoadTextures");
	nTextures = 1;
	format = 0;
	skip = 4;
	dxtex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];

	if (dxtex->rbpp == 8 && dxtex->gbpp == 8 && dxtex->bbpp == 8 && dxtex->abpp == 8)
		format = 1;
	else if (dxtex->rbpp == 5 && dxtex->gbpp == 5 && dxtex->bbpp == 5 && dxtex->abpp == 1)
	{
		format = 2;
		skip = 2;
	}

	if (format <= 1)
	{
		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)MALLOC(compressedSize);
		FileData = (char*)MALLOC(size);

		if (FileCompressed)
		{
			READ(CompressedData, compressedSize, 1, level_fp);
			Decompress(FileData, CompressedData, compressedSize, size);
			READ(&size, 1, 4, level_fp);
			READ(&compressedSize, 1, 4, level_fp);
			SEEK(level_fp, compressedSize, SEEK_CUR);
		}
		else
		{
			READ(FileData, size, 1, level_fp);
			READ(&size, 1, 4, level_fp);
			READ(&compressedSize, 1, 4, level_fp);
			SEEK(level_fp, size, SEEK_CUR);
		}

		FREE(CompressedData);
	}
	else
	{
		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);

		if (FileCompressed)
			SEEK(level_fp, compressedSize, SEEK_CUR);
		else
			SEEK(level_fp, size, SEEK_CUR);

		READ(&size, 1, 4, level_fp);
		READ(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)MALLOC(compressedSize);
		FileData = (char*)MALLOC(size);

		if (FileCompressed)
		{
			READ(CompressedData, compressedSize, 1, level_fp);
			Decompress(FileData, CompressedData, compressedSize, size);
		}
		else
			READ(FileData, size, 1, level_fp);

		FREE(CompressedData);
	}

	pData = FileData;

	Log(5, "RTPages %d", RTPages);
	size = RTPages * skip * 0x10000;
	TextureData = (uchar*)MALLOC(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < RTPages; i++)
	{
		nTex = nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		Textures[nTex].realBump = 0;
		Textures[nTex].staticTex = 0;
		S_LoadBar();
	}

	FREE(TextureData);

	Log(5, "OTPages %d", OTPages);
	size = OTPages * skip * 0x10000;
	TextureData = (uchar*)MALLOC(size);
	memcpy(TextureData, FileData, size);
	FileData += size;
	S_LoadBar();

	for (int i = 0; i < OTPages; i++)
	{
		nTex = nTextures++;
		tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
		DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
		Textures[nTex].tex = pTex;
		Textures[nTex].surface = tSurf;
		Textures[nTex].width = App.TextureSize;
		Textures[nTex].height = App.TextureSize;
		Textures[nTex].bump = 0;
		Textures[nTex].staticTex = 0;
		App.dx.lpD3DDevice->SetTexture(0, pTex);
		S_LoadBar();
	}

	FREE(TextureData);
	S_LoadBar();

	Log(5, "BTPages %d", BTPages);

	if (BTPages)
	{
		size = BTPages * skip * 0x10000;
		TextureData = (uchar*)MALLOC(size);
		memcpy(TextureData, FileData, size);
		FileData += size;

		for (int i = 0; i < BTPages; i++)
		{
			if (i < (BTPages >> 1))
				tSurf = CreateTexturePage(App.TextureSize, App.TextureSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
			else
			{
				if (!App.BumpMapping)
					break;

				tSurf = CreateTexturePage(App.BumpMapSize, App.BumpMapSize, 0, (long*)(TextureData + (i * skip * 0x10000)), 0, format);
			}

			nTex = nTextures++;
			DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
			Textures[nTex].tex = pTex;
			Textures[nTex].surface = tSurf;

			if (i < (BTPages >> 1))
			{
				Textures[nTex].width = App.TextureSize;
				Textures[nTex].height = App.TextureSize;
			}
			else
			{
				Textures[nTex].width = App.BumpMapSize;
				Textures[nTex].height = App.BumpMapSize;
			}

			Textures[nTex].bump = 1;
			Textures[nTex].bumptpage = nTex + (BTPages >> 1);
			Textures[nTex].staticTex = 0;
			S_LoadBar();
		}

		FREE(TextureData);
	}

	FREE(pData);

	READ(&size, 1, 4, level_fp);
	READ(&compressedSize, 1, 4, level_fp);
	CompressedData = (char*)MALLOC(compressedSize);
	FileData = (char*)MALLOC(size);

	if (FileCompressed)
	{
		READ(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);
	}
	else
		READ(FileData, size, 1, level_fp);

	FREE(CompressedData);

	pData = FileData;
	TextureData = (uchar*)MALLOC(0x40000);

	if (!gfCurrentLevel)	//main menu logo
	{
		pComp = 0;
		CompressedData = 0;

		if (Gameflow->Language == US)
			size = LoadFile("data\\uslogo.pak", &CompressedData);
		else if (Gameflow->Language == GERMAN)
			size = LoadFile("data\\grlogo.pak", &CompressedData);
		else if (Gameflow->Language == FRENCH)
			size = LoadFile("data\\frlogo.pak", &CompressedData);
		else
			size = LoadFile("data\\logo512.pak", &CompressedData);

		pComp = (char*)MALLOC(*(long*)CompressedData);
		Decompress(pComp, CompressedData + 4, size - 4, *(long*)CompressedData);
		FREE(CompressedData);

		for (int i = 0; i < 2; i++)
		{
			s = pComp + (i * 768);
			d = (long*)TextureData;

			for (int y = 0; y < 256; y++)
			{
				for (int x = 0; x < 256; x++)
				{
					r = *(s + (x * 3) + (y * 1536));
					g = *(s + (x * 3) + (y * 1536) + 1);
					b = *(s + (x * 3) + (y * 1536) + 2);
					a = 0xFF;

					if (r == 0xFF && b == 0xFF && !g)	//magenta
					{
						r = 0;
						b = 0;
						a = 0;
					}

					c = RGBA(r, g, b, a);
					*d++ = c;
				}
			}

			nTex = nTextures++;
			tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
			DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
			Textures[nTex].tex = pTex;
			Textures[nTex].surface = tSurf;
			Textures[nTex].width = 256;
			Textures[nTex].height = 256;
			Textures[nTex].bump = 0;
			Textures[nTex].staticTex = 0;
		}

		FREE(pComp);
	}

	//shine
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;
	nTex = nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;
	Textures[nTex].staticTex = 0;

	//font
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;
	nTex = nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;
	Textures[nTex].staticTex = 0;

	//sky
	memcpy(TextureData, FileData, 0x40000);
	FileData += 0x40000;
	nTex = nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;
	Textures[nTex].staticTex = 0;

	FREE(TextureData);
	FREE(pData);
	return 1;
}

void S_GetUVRotateTextures()
{
	TEXTURESTRUCT* tex;
	short* pRange;

	pRange = aranges + 1;

	for (int i = 0; i < nAnimUVRanges; i++, pRange++)
	{
		for (int j = (int)*(pRange++); j >= 0; j--, pRange++)
		{
			tex = &textinfo[*pRange];
			AnimatingTexturesV[i][j][0] = tex->v1;
		}

		pRange--;
	}
}

void inject_file(bool replace)
{
	INJECT(0x004A6B30, LoadLevel, 0);

	INJECT(0x004A60E0, LoadTextureInfos, replace);
	INJECT(0x004A4DA0, LoadRooms, replace);
	INJECT(0x004A3CD0, FileOpen, replace);
	INJECT(0x004A3BC0, FindCDDrive, replace);
	INJECT(0x004A3DA0, FileClose, replace);
	INJECT(0x004A3DD0, FileSize, replace);
	INJECT(0x004A6380, LoadItems, replace);
	INJECT(0x004A59D0, LoadSprites, replace);
	INJECT(0x004A5CA0, LoadCameras, replace);
	INJECT(0x004A5D90, LoadSoundEffects, replace);
	INJECT(0x004A6060, LoadAnimatedTextures, replace);
	INJECT(0x004A4E60, LoadObjects, replace);
	INJECT(0x004A67D0, LoadCinematic, replace);
	INJECT(0x004A6880, LoadSamples, replace);
	INJECT(0x004A67F0, LoadAIInfo, replace);
	INJECT(0x004A72B0, S_LoadLevelFile, replace);
	INJECT(0x004A3FC0, LoadTextures, replace);
	INJECT(0x004A6AB0, S_GetUVRotateTextures, replace);
}
