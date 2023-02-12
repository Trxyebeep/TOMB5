#pragma once
#include "types.h"
#include "math_tbls.h"

// Variable macros
#define VAR_(address, type)			(*(type*)(address)) // uninitialized variable
#define ARRAY_(address, type, length)	(*(type(*)length)(address)) // array (can be multidimensional)

//arrays
#define inventry_objects_list	ARRAY_(0x00508E38, INVOBJ, [100])

//defs
#define NO_HEIGHT -32512
#define	NO_ITEM	-1
#define NO_ROOM	255
#define MAX_ITEMS	256
#define MAX_SAMPLES	450
#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)
#define MALLOC_SIZE	5000000		//5MB
#define WINDOW_STYLE	(WS_OVERLAPPED | WS_BORDER | WS_CAPTION)
