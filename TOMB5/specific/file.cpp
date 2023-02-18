#include "../tomb5/pch.h"
#include "../tomb5/libs/zlib/zlib.h"
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
#include "texture.h"
#include "dxshell.h"
#include "lighting.h"
#include "../game/tomb4fx.h"
#include "../game/deltapak.h"
#include "3dmath.h"
#include "specificfx.h"
#include "../game/box.h"
#include "../game/camera.h"
#include "../game/sound.h"
#include "../game/spotcam.h"
#include "../game/control.h"
#include "../game/draw.h"
#include "audio.h"
#include "../game/effects.h"
#include "../game/effect2.h"
#include "../game/lara.h"
#include "drawbars.h"
#include "../game/lot.h"
#include "../tomb5/tomb5.h"

TEXTURESTRUCT* textinfo;
SPRITESTRUCT* spriteinfo;
CHANGE_STRUCT* changes;
RANGE_STRUCT* ranges;
AIOBJECT* AIObjects;
THREAD LevelLoadingThread;
short* aranges;
short* frames;
short* commands;
short* floor_data;
short* mesh_base;
long number_cameras;
long nAnimUVRanges;
short nAIObjects;

TEXTURESTRUCT* AnimatingWaterfalls[6];
float AnimatingWaterfallsV[6];

static TEXTURESTRUCT* MonitorScreenTex;
static FILE* level_fp;
static char* FileData;
static char* CompressedData;
static float MonitorScreenU;
static long FileCompressed = 1;

static char ReadChar()
{
	char read;

	read = *(char*)FileData;
	FileData += sizeof(char);
	return read;
}

static short ReadShort()
{
	short read;

	read = *(short*)FileData;
	FileData += sizeof(short);
	return read;
}

static long ReadLong()
{
	long read;

	read = *(long*)FileData;
	FileData += sizeof(long);
	return read;
}

static void ReadBlock(void* dest, long size)
{
	memcpy(dest, FileData, size);
	FileData += size;
}

FILE* FileOpen(const char* Filename)
{
	FILE* fp;
	char cdFilename[80];

	memset(cdFilename, 0, 80);
	strcat(cdFilename, Filename);
	Log(5, "FileOpen - %s", cdFilename);
	fp = fopen(cdFilename, "rb");

	if (!fp)
		Log(1, "Unable To Open %s", cdFilename);

	return fp;
}

void FileClose(FILE* fp)
{
	Log(2, "FileClose");
	fclose(fp);
}

long FileSize(FILE* fp)
{
	long size;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

long LoadFile(const char* name, char** dest)
{
	FILE* file;
	long size, count;

	Log(2, "LoadFile");
	Log(5, "File - %s", name);
	file = FileOpen(name);

	if (!file)
		return 0;

	size = FileSize(file);

	if (!*dest)
		*dest = (char*)malloc(size);

	count = fread(*dest, 1, size, file);
	Log(5, "Read - %d FileSize - %d", count, size);

	if (count != size)
	{
		Log(1, "Error Reading File");
		FileClose(file);
		free(*dest);
		return 0;
	}

	FileClose(file);
	return size;
}

bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size)
{
	z_stream stream;

	Log(2, "Decompress");
	memset(&stream, 0, sizeof(z_stream));
	stream.avail_in = compressedSize;
	stream.avail_out = size;
	stream.next_out = (Bytef*)pDest;
	stream.next_in = (Bytef*)pCompressed;
	inflateInit(&stream);
	inflate(&stream, Z_FINISH);

	if (stream.total_out != size)
	{
		Log(1, "Error Decompressing Data");
		return 0;
	}

	inflateEnd(&stream);
	Log(5, "Decompression OK");
	return 1;
}

void AdjustUV(long num)
{
	TEXTURESTRUCT* tex;
	float u, v;
	ushort type;

	Log(2, "AdjustUV");

	for (int i = 0; i < num; i++)
	{
		tex = &textinfo[i];
		Textures[tex->tpage].tpage++;
		tex->tpage++;
		u = 1.0F / float(Textures[tex->tpage].width << 1);
		v = 1.0F / float(Textures[tex->tpage].height << 1);
		type = tex->flag & 7;

		if (tex->flag & 0x8000)
		{
			switch (type)
			{
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 2:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 3:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 4:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 5:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 6:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 7:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		}
		else
		{
			switch (type)
			{
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				tex->u4 += u;
				tex->v4 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				tex->u4 -= u;
				tex->v4 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		}
	}
}

void DoMonitorScreen()
{
	float s;
	static long pos, reset;

	if (!MonitorScreenTex)
		return;

	s = (fSin(pos) * 47.0F + 47.0F);
	MonitorScreenTex->u1 = MonitorScreenU + s * (1.0F / 256.0F);
	MonitorScreenTex->u2 = MonitorScreenU + s * (1.0F / 256.0F) + 0.125F;
	MonitorScreenTex->u4 = MonitorScreenU + s * (1.0F / 256.0F);
	MonitorScreenTex->u3 = MonitorScreenU + s * (1.0F / 256.0F) + 0.125F;

	if (reset)
		reset--;
	else
	{
		if (s >= 94 || s <= 0)
			reset = 65;

		pos += 128;
	}
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

void FreeLevel()
{
	MESH_DATA** vbuf;
	MESH_DATA* mesh;

	Log(2, "FreeLevel");

	for (int i = 0; i < num_level_meshes; i++)
	{
		vbuf = &mesh_vtxbuf[i];
		mesh = *vbuf;

		if (mesh->SourceVB)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Mesh VB", mesh->SourceVB, mesh->SourceVB->Release());
			mesh->SourceVB = 0;
		}
	}

	Log(5, "Free Textures");
	FreeTextures();

	DXFreeSounds();
	malloc_ptr = malloc_buffer;
	malloc_free = malloc_size;
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
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)malloc(compressedSize);
		FileData = (char*)malloc(size);

		if (FileCompressed)
		{
			fread(CompressedData, compressedSize, 1, level_fp);
			Decompress(FileData, CompressedData, compressedSize, size);
			fread(&size, 1, 4, level_fp);
			fread(&compressedSize, 1, 4, level_fp);
			fseek(level_fp, compressedSize, SEEK_CUR);
		}
		else
		{
			fread(FileData, size, 1, level_fp);
			fread(&size, 1, 4, level_fp);
			fread(&compressedSize, 1, 4, level_fp);
			fseek(level_fp, size, SEEK_CUR);
		}

		free(CompressedData);
	}
	else
	{
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);

		if (FileCompressed)
			fseek(level_fp, compressedSize, SEEK_CUR);
		else
			fseek(level_fp, size, SEEK_CUR);

		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)malloc(compressedSize);
		FileData = (char*)malloc(size);

		if (FileCompressed)
		{
			fread(CompressedData, compressedSize, 1, level_fp);
			Decompress(FileData, CompressedData, compressedSize, size);
		}
		else
			fread(FileData, size, 1, level_fp);

		free(CompressedData);
	}

	pData = FileData;

	Log(5, "RTPages %d", RTPages);
	size = RTPages * skip * 0x10000;
	TextureData = (uchar*)malloc(size);
	ReadBlock(TextureData, size);
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

	free(TextureData);

	Log(5, "OTPages %d", OTPages);
	size = OTPages * skip * 0x10000;
	TextureData = (uchar*)malloc(size);
	ReadBlock(TextureData, size);
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

	free(TextureData);
	S_LoadBar();

	Log(5, "BTPages %d", BTPages);

	if (BTPages)
	{
		size = BTPages * skip * 0x10000;
		TextureData = (uchar*)malloc(size);
		ReadBlock(TextureData, size);

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

		free(TextureData);
	}

	free(pData);

	fread(&size, 1, 4, level_fp);
	fread(&compressedSize, 1, 4, level_fp);
	CompressedData = (char*)malloc(compressedSize);
	FileData = (char*)malloc(size);

	if (FileCompressed)
	{
		fread(CompressedData, compressedSize, 1, level_fp);
		Decompress(FileData, CompressedData, compressedSize, size);
	}
	else
		fread(FileData, size, 1, level_fp);

	free(CompressedData);

	pData = FileData;
	TextureData = (uchar*)malloc(0x40000);

	//main menu logo
	if (gfCurrentLevel == LVL5_TITLE)
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

		pComp = (char*)malloc(*(long*)CompressedData);
		Decompress(pComp, CompressedData + 4, size - 4, *(long*)CompressedData);
		free(CompressedData);

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

		free(pComp);
	}

	//shine
	ReadBlock(TextureData, 0x40000);
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
	ReadBlock(TextureData, 0x40000);
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
	ReadBlock(TextureData, 0x40000);
	nTex = nTextures++;
	tSurf = CreateTexturePage(256, 256, 0, (long*)TextureData, 0, 0);
	DXAttempt(tSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pTex));
	Textures[nTex].tex = pTex;
	Textures[nTex].surface = tSurf;
	Textures[nTex].width = 256;
	Textures[nTex].height = 256;
	Textures[nTex].bump = 0;
	Textures[nTex].staticTex = 0;

	free(TextureData);
	free(pData);
	return 1;
}

static void aLoadRoomStream()
{
	ROOM_INFO* r;
	long num, size;
	char* data;

	ReadLong();	//skip unused space
	num = ReadLong();
	room = (ROOM_INFO*)game_malloc(num * sizeof(ROOM_INFO));

	for (int i = 0; i < num; i++)
	{
		r = &room[i];

		ReadLong();		//X E L A

		size = ReadLong();
		data = (char*)game_malloc(size);
		ReadBlock(data, size);
		aFixUpRoom(r, data);
	}

	number_rooms = (short)num;
}

bool LoadRooms()
{
	long size;

	Log(2, "LoadRooms");
	wibble = 0;
	MaxRoomLights = 0;

	aLoadRoomStream();
	BuildOutsideTable();

	size = ReadLong();
	floor_data = (short*)game_malloc(size * sizeof(short));
	ReadBlock(floor_data, sizeof(short) * size);

	Log(0, "Floor Data Size %d @ %x", size, floor_data);
	return 1;
}

bool LoadObjects()
{
	OBJECT_INFO* obj;
	STATIC_INFO* stat;
	short** mesh;
	short** mesh_size;
	long num, slot, lp;
	static long num_meshes;
	static long num_anims;

	Log(2, "LoadObjects");
	memset(objects, 0, NUMBER_OBJECTS * sizeof(OBJECT_INFO));
	memset(static_objects, 0, 70 * sizeof(STATIC_INFO));

	//meshes
	num = ReadLong();
	mesh_base = (short*)game_malloc(num * 2);
	ReadBlock(mesh_base, num * sizeof(short));

	num = ReadLong();
	meshes = (short**)game_malloc(num * sizeof(long) * 2);	//*2 for meshswaps
	ReadBlock(meshes, num * sizeof(long));

	for (lp = 0; lp < num; lp++)
		meshes[lp] = &mesh_base[(long)meshes[lp] / 2];

	num_meshes = num;

	//anims
	num_anims = ReadLong();
	anims = (ANIM_STRUCT*)game_malloc(num_anims * sizeof(ANIM_STRUCT));
	ReadBlock(anims, num_anims * sizeof(ANIM_STRUCT));

	//changes
	num = ReadLong();
	changes = (CHANGE_STRUCT*)game_malloc(num * sizeof(CHANGE_STRUCT));
	ReadBlock(changes, num * sizeof(CHANGE_STRUCT));

	//ranges
	num = ReadLong();
	ranges = (RANGE_STRUCT*)game_malloc(num * sizeof(RANGE_STRUCT));
	ReadBlock(ranges, num * sizeof(RANGE_STRUCT));

	//anim commands
	num = ReadLong();
	commands = (short*)game_malloc(num * sizeof(short));
	ReadBlock(commands, num * sizeof(short));

	//bones
	num = ReadLong();
	bones = (long*)game_malloc(num * sizeof(long));
	ReadBlock(bones, num * sizeof(long));

	//frames
	num = ReadLong();
	frames = (short*)game_malloc(num * sizeof(short));
	ReadBlock(frames, num * sizeof(short));

	for (lp = 0; lp < num_anims; lp++)
		anims[lp].frame_ptr = (short*)((long)anims[lp].frame_ptr + (long)frames);

	num = ReadLong();

	for (lp = 0; lp < num; lp++)
	{
		slot = ReadLong();
		obj = &objects[slot];

		obj->nmeshes = ReadShort();
		obj->mesh_index = ReadShort();
		obj->bone_index = ReadLong();
		obj->frame_base = (short*)ReadLong();
		obj->anim_index = ReadShort();
		obj->loaded = 1;

		ReadShort();	//Skip padding
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
		*mesh++ = *mesh_size;
		*mesh++ = *mesh_size++;
	}

	InitialiseObjects();
	InitialiseClosedDoors();

	//statics
	num = ReadLong();

	for (int i = 0; i < num; i++)
	{
		slot = ReadLong();
		stat = &static_objects[slot];

		stat->mesh_number = ReadShort();
		ReadBlock(&stat->x_minp, 6 * sizeof(short));
		ReadBlock(&stat->x_minc, 6 * sizeof(short));
		stat->flags = ReadShort();
	}

	for (int i = 0; i < 70; i++)
	{
		stat = &static_objects[i];
		stat->mesh_number *= 2;
	}

	ProcessMeshData(num_meshes * 2);
	return 1;
}

bool LoadSprites()
{
	STATIC_INFO* stat;
	OBJECT_INFO* obj;
	PHDSPRITESTRUCT sprite;
	long num_sprites, num_slots, slot;

	Log(2, "LoadSprites");
	ReadLong();			//SPR 0 marker

	num_sprites = ReadLong();
	spriteinfo = (SPRITESTRUCT*)game_malloc(sizeof(SPRITESTRUCT) * num_sprites);

	for (int i = 0; i < num_sprites; i++)
	{
		ReadBlock(&sprite, sizeof(PHDSPRITESTRUCT));
		spriteinfo[i].height = sprite.height;
		spriteinfo[i].offset = sprite.offset;
		spriteinfo[i].tpage = sprite.tpage;
		spriteinfo[i].width = sprite.width;
		spriteinfo[i].x1 = (float)sprite.x1 * (1.0F / 256.0F);
		spriteinfo[i].y1 = (float)sprite.y1 * (1.0F / 256.0F);
		spriteinfo[i].x2 = (float)sprite.x2 * (1.0F / 256.0F);
		spriteinfo[i].y2 = (float)sprite.y2 * (1.0F / 256.0F);
		spriteinfo[i].x1 += (1.0F / 256.0F);
		spriteinfo[i].y1 += (1.0F / 256.0F);
		spriteinfo[i].x2 -= (1.0F / 256.0F);
		spriteinfo[i].y2 -= (1.0F / 256.0F);
		spriteinfo[i].tpage++;
	}

	num_slots = ReadLong();

	if (num_slots <= 0)
		return 1;

	for (int i = 0; i < num_slots; i++)
	{
		slot = ReadLong();

		if (slot >= NUMBER_OBJECTS)
		{
			slot -= NUMBER_OBJECTS;
			stat = &static_objects[slot];
			stat->mesh_number = ReadShort();
			stat->mesh_number = ReadShort();
		}
		else
		{
			obj = &objects[slot];
			obj->nmeshes = ReadShort();
			obj->mesh_index = ReadShort();
			obj->loaded = 1;
		}
	}

	return 1;
}

bool LoadCameras()
{
	Log(2, "LoadCameras");
	number_cameras = ReadLong();

	if (number_cameras)
	{
		camera.fixed = (OBJECT_VECTOR*)game_malloc(number_cameras * sizeof(OBJECT_VECTOR));
		ReadBlock(camera.fixed, number_cameras * sizeof(OBJECT_VECTOR));
	}

	number_spotcams = (short)ReadLong();

	if (number_spotcams)
		ReadBlock(SpotCam, number_spotcams * sizeof(SPOTCAM));

	return 1;
}

bool LoadSoundEffects()
{
	Log(2, "LoadSoundEffects");
	number_sound_effects = ReadLong();
	Log(8, "Number of SFX %d", number_sound_effects);

	if (number_sound_effects)
	{
		sound_effects = (OBJECT_VECTOR*)game_malloc(number_sound_effects * sizeof(OBJECT_VECTOR));
		ReadBlock(sound_effects, number_sound_effects * sizeof(OBJECT_VECTOR));
	}

	return 1;
}

bool LoadBoxes()
{
	BOX_INFO* box;
	long size;

	Log(2, "LoadBoxes");
	num_boxes = ReadLong();

	boxes = (BOX_INFO*)game_malloc(num_boxes * sizeof(BOX_INFO));
	ReadBlock(boxes, num_boxes * sizeof(BOX_INFO));

	size = ReadLong();
	overlap = (ushort*)game_malloc(size * sizeof(ushort));
	ReadBlock(overlap, size * sizeof(ushort));

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ground_zone[j][i] = (short*)game_malloc(num_boxes * sizeof(short));
			ReadBlock(ground_zone[j][i], num_boxes * sizeof(short));
		}

		ground_zone[4][i] = (short*)game_malloc(num_boxes * sizeof(short));
		ReadBlock(ground_zone[4][i], num_boxes * sizeof(short));
	}

	for (int i = 0; i < num_boxes; i++)
	{
		box = &boxes[i];

		if (box->overlap_index & 0x8000)
			box->overlap_index |= 0x4000;
	}

	return 1;
}

bool LoadAnimatedTextures()
{
	long num_anim_ranges;

	num_anim_ranges = ReadLong();

	aranges = (short*)game_malloc(num_anim_ranges * 2);
	ReadBlock(aranges, num_anim_ranges * sizeof(short));

	if (gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS)
		aranges[2] = 2076;

	nAnimUVRanges = ReadChar();
	return 1;
}

bool LoadTextureInfos()
{
	long val;
	PHDTEXTURESTRUCT tex;

	Log(2, "LoadTextureInfos");

	ReadLong();
	val = ReadLong();
	Log(5, "Texture Infos : %d", val);
	textinfo = (TEXTURESTRUCT*)game_malloc(val * sizeof(TEXTURESTRUCT));

	for (int i = 0; i < val; i++)
	{
		ReadBlock(&tex, sizeof(PHDTEXTURESTRUCT));
		textinfo[i].drawtype = tex.drawtype;
		textinfo[i].tpage = tex.tpage & 0x7FFF;
		textinfo[i].flag = tex.tpage ^ (tex.tpage ^ tex.flag) & 0x7FFF;

		if ((gfCurrentLevel == LVL5_STREETS_OF_ROME && (i == 200 || i == 204)) ||
			(gfCurrentLevel == LVL5_TRAJAN_MARKETS && (i == 225 || i == 229)) ||
			(gfCurrentLevel == LVL5_COLOSSEUM && (i == 244 || i == 240)) ||
			(gfCurrentLevel == LVL5_BASE && (i == 213 || i == 210)) ||
			(gfCurrentLevel == LVL5_SUBMARINE && (i == 205 || i == 208)) ||
			(gfCurrentLevel == LVL5_DEEPSEA_DIVE && (i == 201 || i == 205)) ||
			(gfCurrentLevel == LVL5_SINKING_SUBMARINE && (i == 238 || i == 235)) ||
			((gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL) && (i == 148 || i == 144)) ||
			(gfCurrentLevel == LVL5_THIRTEENTH_FLOOR && (i == 99 || i == 103)) ||
			(gfCurrentLevel == LVL5_ESCAPE_WITH_THE_IRIS && (i == 105 || i == 101)) ||
			(gfCurrentLevel == LVL5_RED_ALERT && (i == 133 || i == 137)))
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

bool LoadItems()
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	STATIC_INFO* stat;
	long x, y, z;
	static long num_items;

	Log(2, "LoadItems");
	num_items = ReadLong();

	if (!num_items)
		return 1;

	items = (ITEM_INFO*)game_malloc(MAX_ITEMS * sizeof(ITEM_INFO));
	level_items = num_items;
	InitialiseClosedDoors();
	InitialiseItemArray(MAX_ITEMS);

	for (int i = 0; i < num_items; i++)
	{
		item = &items[i];
		item->object_number = ReadShort();
		item->room_number = ReadShort();
		item->pos.x_pos = ReadLong();
		item->pos.y_pos = ReadLong();
		item->pos.z_pos = ReadLong();
		item->pos.y_rot = ReadShort();
		item->shade = ReadShort();
		item->trigger_flags = ReadShort();
		item->flags = ReadShort();
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

			if (boxes[floor->box].overlap_index & 0x4000)
				continue;

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

	return 1;
}

bool LoadAIInfo()
{
	long num;

	num = ReadLong();

	if (!num)
		return 1;

	nAIObjects = (short)num;
	AIObjects = (AIOBJECT*)game_malloc(num * sizeof(AIOBJECT));
	ReadBlock(AIObjects, num * sizeof(AIOBJECT));
	return 1;
}

bool LoadCinematic()
{
	ReadShort();
	return 1;
}

bool LoadSamples()
{
	long num_samples, uncomp_size, comp_size;
	static long num_sample_infos;

	Log(2, "LoadSamples");
	sample_lut = (short*)game_malloc(MAX_SAMPLES * sizeof(short));
	ReadBlock(sample_lut, MAX_SAMPLES * sizeof(short));


	num_sample_infos = ReadLong();
	Log(8, "Number Of Sample Infos %d", num_sample_infos);

	if (!num_sample_infos)
	{
		Log(1, "No Sample Infos");
		return 0;
	}

	sample_infos = (SAMPLE_INFO*)game_malloc(num_sample_infos * sizeof(SAMPLE_INFO));
	ReadBlock(sample_infos, num_sample_infos * sizeof(SAMPLE_INFO));

	num_samples = ReadLong();

	if (!num_samples)
	{
		Log(1, "No Samples");
		return 0;
	}

	Log(8, "Number Of Samples %d", num_samples);
	fread(&num_samples, 1, 4, level_fp);

	if (feof(level_fp))
		Log(1, "END OF FILE");

	InitSampleDecompress();

	if (num_samples <= 0)
	{
		FreeSampleDecompress();
		return 1;
	}

	for (int i = 0; i < num_samples; i++)
	{
		fread(&uncomp_size, 1, 4, level_fp);
		fread(&comp_size, 1, 4, level_fp);
		fread(samples_buffer, comp_size, 1, level_fp);

		if (!DXCreateSampleADPCM(samples_buffer, comp_size, uncomp_size, i))
		{
			FreeSampleDecompress();
			return 0;
		}
	}

	FreeSampleDecompress();
	return 1;
}

unsigned int __stdcall LoadLevel(void* name)
{
	OBJECT_INFO* obj;
	TEXTURESTRUCT* tex;
	MESH_DATA* mesh;
	char* pData;
	char* pBefore;
	char* pAfter;
	long version, size, compressedSize;
	short RTPages, OTPages, BTPages;
	short data[16];

	Log(5, "Begin LoadLevel");
	FreeLevel();
	InitialiseFXArray(1);
	InitialiseLOTarray(1);
	nTextures = 1;
	Textures[0].tex = 0;
	Textures[0].surface = 0;
	Textures[0].width = 0;
	Textures[0].height = 0;
	Textures[0].bump = 0;
	CompressedData = 0;
	FileData = 0;
	level_fp = 0;
	level_fp = FileOpen((const char*)name);

	if (level_fp)
	{
		fread(&version, 1, 4, level_fp);
		fread(&RTPages, 1, 2, level_fp);
		fread(&OTPages, 1, 2, level_fp);
		fread(&BTPages, 1, 2, level_fp);
		S_InitLoadBar(OTPages + BTPages + RTPages + 20);
		S_LoadBar();

		Log(7, "Process Level Data");
		LoadTextures(RTPages, OTPages, BTPages);

		fread(data, 1, 32, level_fp);
		LaraDrawType = data[0] + LARA_NORMAL;
		WeatherType = (char)data[1];

		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		FileData = (char*)malloc(size);
		fread(FileData, size, 1, level_fp);

		pData = FileData;

		Log(5, "Rooms");
		LoadRooms();
		S_LoadBar();

		Log(5, "Objects");
		LoadObjects();
		S_LoadBar();

		LoadSprites();
		S_LoadBar();

		LoadCameras();
		S_LoadBar();

		LoadSoundEffects();
		S_LoadBar();

		LoadBoxes();
		S_LoadBar();

		LoadAnimatedTextures();
		S_LoadBar();

		LoadTextureInfos();
		S_LoadBar();

		pBefore = FileData;

		size = ReadLong();		//nItems
		FileData += 24 * size;	//skip item data
		LoadAIInfo();

		pAfter = FileData;
		FileData = pBefore;

		LoadItems();
		FileData = pAfter;

		S_LoadBar();
		S_LoadBar();

		LoadCinematic();
		S_LoadBar();

		if (acm_ready && !App.SoundDisabled)
			LoadSamples();

		free(pData);
		S_LoadBar();

		for (int i = 0; i < 6; i++)
		{
			obj = &objects[WATERFALL1 + i];

			if (obj->loaded)
			{
				tex = &textinfo[mesh_vtxbuf[obj->mesh_index]->gt4[4] & 0x7FFF];
				AnimatingWaterfalls[i] = tex;
				AnimatingWaterfallsV[i] = tex->v1;
			}
		}

		S_LoadBar();

		S_GetUVRotateTextures();
		S_LoadBar();
		SetupGame();
		S_LoadBar();
		SetFadeClip(0, 1);
		reset_cutseq_vars();

		if (gfCurrentLevel == LVL5_STREETS_OF_ROME)
			find_a_fucking_item(ANIMATING10)->mesh_bits = 11;

		if (gfCurrentLevel == LVL5_OLD_MILL)
			find_a_fucking_item(ANIMATING16)->mesh_bits = 1;

		MonitorScreenTex = 0;
		obj = &objects[MONITOR_SCREEN];

		if (obj->loaded)
		{
			mesh = (MESH_DATA*)meshes[objects[MONITOR_SCREEN].mesh_index];

			for (int i = 0; i < mesh->ngt4; i++)
			{
				if (mesh->gt4[i * 6 + 5] & 1)	//semitrans quad
				{
					mesh->gt4[i * 6 + 5] &= ~1;	//no more
					MonitorScreenTex = &textinfo[mesh->gt4[i * 6 + 4] & 0x7FFF];
					MonitorScreenU = MonitorScreenTex->u1;
					break;
				}
			}
		}

		FileClose(level_fp);
		aInit();
		aInitMatrix();
		ClearFX();
	}

	aMakeCutsceneResident(gfResidentCut[0], gfResidentCut[1], gfResidentCut[2], gfResidentCut[3]);
	LevelLoadingThread.active = 0;
	_endthreadex(1);
	return 1;
}

long S_LoadLevelFile(long num)
{
	static long lscreen = 0;
	static long flag = 0;
	long chosen_screen;
	char name[80];

	Log(2, "S_LoadLevelFile");

	if (!tomb5.tr4_loadscreens || (!num && !bDoCredits && !gfStatus))
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
		DrawSortList();
		S_DumpScreenFrame();
	}

	if (!tomb5.tr4_loadscreens || (!num && !bDoCredits && !gfStatus))
	{
		if (MonoScreenOn == 2)
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
