#pragma once

#pragma pack(push, 1)
typedef struct 
{
	BYTE opCode;	// must be 0xE9;
	DWORD offset;	// jump offset
} JMP;

#define INJECT(from,to,replace) \
do \
{ \
	if (replace) \
		INJECT_JMP(from,to); \
	else \
		INJECT_JMP(to,from); \
} while (false)

#define INJECT_JMP(from,to) \
do \
{ \
	((JMP*)(from))->opCode = 0xE9; \
	((JMP*)(from))->offset = (DWORD)(to) - ((DWORD)(from) + sizeof(JMP)); \
} while (false)

#ifndef ABS
#define ABS(x) (((x)<0) ? (-(x)) : (x))
#endif // ABS

#ifndef phd_sin
#define phd_sin(x) (4 * rcossin_tbl[((int)(x) >> 3) & 0x1FFE])
#endif // phd_sin

#ifndef phd_cos
#define phd_cos(x) (4 * rcossin_tbl[(((int)(x) >> 3) & 0x1FFE) + 1])
#endif // phd_cos

#ifndef SQUARE
#define SQUARE(x) ((x)*(x))
#endif // SQUARE

#define phd_PopMatrix()		{phd_mxptr -= 12; aMXPtr -= 12;}

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#define RGBONLY(r, g, b) ((b & 0xFF) | (((g & 0xFF) | ((r & 0xFF) << 8)) << 8))
#define RGBA(r, g, b, a) (RGBONLY(r, g, b) | ((a) << 24))
#define ARGB(r, g, b, a) (RGBA(b, g, r, a))
#define RGB_M(clr, m) (clr = (clr & 0xFF000000) | (((((clr >> 16) & 0xFF) * m) >> 8) << 16) | (((((clr >> 8) & 0xFF)* m) >> 8) << 8) | (((clr & 0xFF) * m) >> 8))
//font flags for PrintString
#define FF_SMALL	0x1000
#define FF_BLINK	0x2000
#define FF_RJUSTIFY	0x4000
#define FF_CENTER	0x8000

enum lara_draw_type
{
	LARA_NORMAL = 1,
	LARA_YOUNG = 2,
	LARA_BUNHEAD = 3,
	LARA_CATSUIT = 4,
	LARA_DIVESUIT = 5,
	LARA_INVISIBLE = 7
};

enum lara_mesh
{
	LM_HIPS = 0,
	LM_LTHIGH = 1,
	LM_LSHIN = 2,
	LM_LFOOT = 3,
	LM_RTHIGH = 4,
	LM_RSHIN = 5,
	LM_RFOOT = 6,
	LM_TORSO = 7,
	LM_RINARM = 8,
	LM_ROUTARM = 9,
	LM_RHAND = 10,
	LM_LINARM = 11,
	LM_LOUTARM = 12,
	LM_LHAND = 13,
	LM_HEAD = 14,

	NUM_LARA_MESHES
};

enum item_status
{
	ITEM_INACTIVE = 0,
	ITEM_ACTIVE = 1,
	ITEM_DEACTIVATED = 2,
	ITEM_INVISIBLE = 3
};

enum lara_gun_status
{
	LG_NO_ARMS = 0,
	LG_HANDS_BUSY = 1,
	LG_DRAW_GUNS = 2,
	LG_UNDRAW_GUNS = 3,
	LG_READY = 4,
	LG_FLARE = 5,
};

enum lara_water_status
{
	LW_ABOVE_WATER = 0,
	LW_UNDERWATER = 1,
	LW_SURFACE = 2,
	LW_FLYCHEAT = 3,
	LW_WADE = 4
};

enum ITEM_FLAGS
{
	IFL_TRIGGERED =				0x20,
	IFL_SWITCH_ONESHOT =		0x40,	//oneshot for switch items
	IFL_ANTITRIGGER_ONESHOT =	0x80,	//oneshot for antitriggers
	IFL_INVISIBLE =				0x100,	//also used as oneshot for everything else
	IFL_CODEBITS =				0x3E00,
	IFL_REVERSE =				0x4000,	
	IFL_CLEARBODY =				0x8000
};

enum room_flags
{
	ROOM_UNDERWATER = 1,
	ROOM_SFX_ALWAYS = 2,
	ROOM_PITCH_SHIFT = 4,
	ROOM_OUTSIDE = 8,
	ROOM_DYNAMIC_LIT = 16,
	ROOM_NOT_INSIDE = 32,
	ROOM_INSIDE = 64,
	ROOM_NO_LENSFLARE = 128
};

enum collision_types
{
	CT_NONE = 0,
	CT_FRONT = (1 << 0),
	CT_LEFT = (1 << 1),
	CT_RIGHT = (1 << 2),
	CT_TOP = (1 << 3),
	CT_TOP_FRONT = (1 << 4),
	CT_CLAMP = (1 << 5)
};

enum input_buttons
{
	IN_NONE = 0,								// 0x00000000
	IN_FORWARD = (1 << 0),						// 0x00000001
	IN_BACK = (1 << 1),							// 0x00000002
	IN_LEFT = (1 << 2),							// 0x00000004
	IN_RIGHT = (1 << 3),						// 0x00000008
	IN_JUMP = (1 << 4),							// 0x00000010
	IN_DRAW = (1 << 5),							// 0x00000020
	IN_ACTION = (1 << 6),						// 0x00000040
	IN_WALK = (1 << 7),							// 0x00000080
	IN_OPTION = (1 << 8),						// 0x00000100
	IN_LOOK = (1 << 9),							// 0x00000200
	IN_LSTEP = (1 << 10),						// 0x00000400
	IN_RSTEP = (1 << 11),						// 0x00000800
	IN_ROLL = (1 << 12),						// 0x00001000
	IN_PAUSE = (1 << 13),						// 0x00002000
	IN_A = (1 << 14),							// 0x00004000
	IN_B = (1 << 15),							// 0x00008000
	IN_CHEAT = (1 << 16),						// 0x00010000
	IN_D = (1 << 17),							// 0x00020000
	IN_E = (1 << 18),							// 0x00040000
	IN_FLARE = (1 << 19),						// 0x00080000
	IN_SELECT = (1 << 20),						// 0x00100000
	IN_DESELECT = (1 << 21),					// 0x00200000
	IN_SAVE = (1 << 22), // F5					// 0x00400000
	IN_LOAD = (1 << 23),  // F6					// 0x00800000
	IN_STEPSHIFT = (1 << 24),					// 0x01000000
	IN_LOOKLEFT = (1 << 25),					// 0x02000000
	IN_LOOKRIGHT = (1 << 26),					// 0x04000000
	IN_LOOKFORWARD = (1 << 27),					// 0x08000000
	IN_LOOKBACK = (1 << 28),					// 0x10000000
	IN_DUCK = (1 << 29),						// 0x20000000
	IN_SPRINT = (1 << 30),						// 0x40000000
	IN_TARGET = (1 << 31),						// 0x80000000

	IN_ALL = ~0,								// 0xFFFFFFFF (-1)
};

enum height_types
{
	WALL,
	SMALL_SLOPE,
	BIG_SLOPE,
	DIAGONAL,
	SPLIT_TRI
};

enum floor_types 
{
	FLOOR_TYPE, DOOR_TYPE, TILT_TYPE, ROOF_TYPE, TRIGGER_TYPE, LAVA_TYPE, CLIMB_TYPE, SPLIT1, SPLIT2, SPLIT3, SPLIT4,
	NOCOLF1T, NOCOLF1B, NOCOLF2T, NOCOLF2B, NOCOLC1T, NOCOLC1B, NOCOLC2T, NOCOLC2B,
	MONKEY_TYPE, TRIGTRIGGER_TYPE, MINER_TYPE
};

enum weapon_type_carried 
{
	WTYPE_MISSING = 0,
	WTYPE_PRESENT = 1,
	WTYPE_SILENCER = 2,
	WTYPE_LASERSIGHT = 4,
	WTYPE_AMMO_1 = 8,
	WTYPE_AMMO_2 = 16,
	WTYPE_AMMO_3 = 32,

	WTYPE_MASK_AMMO = WTYPE_AMMO_1 | WTYPE_AMMO_2 | WTYPE_AMMO_3
};

enum zone_type
{
	SKELLY_ZONE = 0,
	BASIC_ZONE = 1,
	CROC_ZONE = 2,
	HUMAN_ZONE = 3,
	FLYER_ZONE = 4,
};

enum camera_type
{
	CHASE_CAMERA = 0,
	FIXED_CAMERA = 1,
	LOOK_CAMERA = 2,
	COMBAT_CAMERA = 3,
	CINEMATIC_CAMERA = 4,
	HEAVY_CAMERA = 5,
};

enum mood_type
{
	BORED_MOOD = 0,
	ATTACK_MOOD = 1,
	ESCAPE_MOOD = 2,
	STALK_MOOD = 3,
};

enum weapon_types 
{
	WEAPON_NONE = 0,
	WEAPON_PISTOLS = 1,
	WEAPON_REVOLVER = 2,
	WEAPON_UZI = 3,
	WEAPON_SHOTGUN = 4,
	WEAPON_HK = 5,
	WEAPON_CROSSBOW = 6,
	WEAPON_FLARE = 7,
	WEAPON_TORCH = 8
};

enum quadrant_names
{
	NORTH,
	EAST,
	SOUTH,
	WEST
};

enum cloth_type
{
	CLOTH_MISSING = 0,
	CLOTH_DRY = 1,
	CLOTH_WET = 2
};

enum trigger_types 
{
	TRIGGER, PAD, SWITCH, KEY, PICKUP, HEAVY, ANTIPAD, COMBAT, DUMMY, ANTITRIGGER, HEAVYSWITCH, HEAVYANTITRIGGER, MONKEY, SKELETON_T, TIGHTROPE_T, CRAWLDUCK_T, CLIMB_T
};

enum trigobj_types 
{
	TO_OBJECT, TO_CAMERA, TO_SINK, TO_FLIPMAP, TO_FLIPON, TO_FLIPOFF, TO_TARGET, TO_FINISH, TO_CD, TO_FLIPEFFECT, TO_SECRET, TO_BODYBAG, TO_FLYBY, TO_CUTSCENE
};

struct OBJECT_VECTOR
{
	long x;
	long y;
	long z;
	short data;
	short flags;
};

struct bounding_box_f
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
};

struct SUBSUIT_INFO
{
	short XRot;
	short dXRot;
	short XRotVel;
	short Vel[2];
	short YVel;
};

struct ROOMLET
{
	short	nVtx;
	short	nWVtx;
	short	nSVtx;
	short	nQuad;
	short	nTri;
	short	nWQuad;
	short	nWTri;
	short	padd;
	float	bBox[6];
	LPDIRECT3DVERTEXBUFFER	pVtx;
	float* pSVtx;
	short* pFac;
	int* pPrelight;
};

struct SPHERE
{
	long x;
	long y;
	long z;
	long r;
};


struct GAME_VECTOR
{
	long x;
	long y;
	long z;
	short room_number;
	short box_number;
};

struct PHD_VECTOR
{
	long x;
	long y;
	long z;
};

struct PHD_3DPOS
{
	long x_pos;
	long y_pos;
	long z_pos;
	short x_rot;
	short y_rot;
	short z_rot;
};

struct SVECTOR
{
	short vx;
	short vy;
	short vz;
	short pad;
};

struct CVECTOR
{
	char b;
	char g;
	char r;
	char a;
};

struct CHANGE_STRUCT
{
	short goal_anim_state;
	short number_ranges;
	short range_index;
};

struct HAIR_STRUCT
{
	PHD_3DPOS pos;
	PHD_VECTOR vel;
	float fx;
	float fy;
	float fz;
};

struct DISPLAYPU
{
	short life;
	short object_number;
};

struct RANGE_STRUCT
{
	short start_frame;
	short end_frame;
	short link_anim_num;
	short link_frame_num;
};

struct WEAPON_INFO
{
	short lock_angles[4];
	short left_angles[4];
	short right_angles[4];
	short aim_speed;
	short shot_accuracy;
	short gun_height;
	short target_dist;
	char damage;
	char recoil_frame;
	char flash_time;
	char draw_frame;
	short sample_num;
};

struct GAMEFLOW
{
	unsigned int CheatEnabled : 1;
		unsigned int LoadSaveEnabled : 1;
		unsigned int TitleEnabled : 1;
		unsigned int PlayAnyLevel : 1;
		unsigned int Language : 3;
		unsigned int DemoDisc : 1;
		unsigned int Unused : 24;
	unsigned int InputTimeout;
	uchar SecurityTag;
	uchar nLevels;
	uchar nFileNames;
	uchar Pad;
	ushort FileNameLen;
	ushort ScriptLen;
};

struct FLOOR_INFO
{
	ushort index;
	ushort fx : 4;
		ushort box : 11;
		ushort stopper : 1;
	uchar pit_room;
	char floor;
	uchar sky_room;
	char ceiling;
};

struct ANIM_STRUCT
{
	short* frame_ptr;
	short interpolation;
	short current_anim_state;
	long velocity;
	long acceleration;
	long Xvelocity;
	long Xacceleration;
	short frame_base;
	short frame_end;
	short jump_anim_num;
	short jump_frame_num;
	short number_changes;
	short change_index;
	short number_commands;
	short command_index;
};

struct PCLIGHT//evil bitch
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	float tr;
	float tg;
	float tb;
	float rs;
	float gs;
	float bs;
	long fcnt;
	uchar Type;
	uchar Active;
	PHD_VECTOR rlp;
	long Range;
};

struct ITEM_LIGHT
{
	long r;
	long g;
	long b;
	long ambient;
	long rs;
	long gs;
	long bs;
	long fcnt;
	PCLIGHT	CurrentLights[21];
	PCLIGHT	PrevLights[21];
	int nCurrentLights;
	int nPrevLights;
	int	room_number;
	int RoomChange;
	PHD_VECTOR item_pos;
	void* pCurrentLights;
	void* pPrevLights;
};

struct SAMPLE_INFO
{
	short number;
	uchar volume;
	char radius;
	char randomness;
	char pitch;
	short flags;
};

struct SoundSlot
{
	int OrigVolume;
	int nVolume;
	int nPan;
	int nPitch;
	int nSampleInfo;
	ulong distance;
	PHD_VECTOR pos;
};

struct box_node
{
	short exit_box;
	ushort search_number;
	short next_expansion;
	short box_number;
};

struct LIGHTINFO
{
	long x;
	long y;
	long z;
	uchar r;
	uchar g;
	uchar b;
	uchar Type;
	short Intensity;
	float Inner;
	float Outer;
	short Length;
	short Cutoff;
	float nx;
	float ny;
	float nz;
};

struct MESH_INFO
{
	long x;
	long y;
	long z;
	short y_rot;
	short shade;
	short Flags;
	short static_number;
};

struct ITEM_INFO
{
	long floor;
	ulong touch_bits;
	ulong mesh_bits;
	short object_number;
	short current_anim_state;
	short goal_anim_state;
	short required_anim_state;
	short anim_number;
	short frame_number;
	short room_number;
	short next_item;
	short next_active;
	short speed;
	short fallspeed;
	short hit_points;
	ushort box_number;
	short timer;
	short flags;
	short shade;
	short trigger_flags;
	short carried_item;
	short after_death;
	ushort fired_weapon;
	short item_flags[4];
	void* data;
	PHD_3DPOS pos;
	ITEM_LIGHT il;
	ulong active : 1;
	ulong status : 2;
	ulong gravity_status : 1;
	ulong hit_status : 1;
	ulong collidable : 1;
	ulong looked_at : 1;
	ulong dynamic_light : 1;
	ulong poisoned : 1;
	ulong ai_bits : 5;
	ulong really_active : 1;
	ulong InDrawRoom : 1;
	ulong meshswap_meshbits;
	short draw_room;
	short TOSSPAD;
};

struct LARA_ARM
{
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
};

struct FX_INFO
{
	PHD_3DPOS pos;
	short room_number;
	short object_number;
	short next_fx;
	short next_active;
	short speed;
	short fallspeed;
	short frame_number;
	short counter;
	short shade;
	short flag1;
	short flag2;
};

struct LOT_INFO
{
	box_node* node;
	short head;
	short tail;
	ushort search_number;
	ushort block_mask;
	short step;
	short drop;
	short zone_count;
	short target_box;
	short required_box;
	short fly;
	ushort can_jump : 1;
	ushort can_monkey : 1;
	ushort is_amphibious : 1;
	ushort is_jumping : 1;
	ushort is_monkeying : 1;
	PHD_VECTOR target;
	zone_type zone;
};

struct FOOTPRINT
{
	long x;
	long y;
	long z;
	short YRot;
	short Active;
};

struct CREATURE_INFO
{
	short joint_rotation[4];
	short maximum_turn;
	short flags;
	ushort alerted : 1;
	ushort head_left : 1;
	ushort head_right : 1;
	ushort reached_goal : 1;
	ushort hurt_by_lara : 1;
	ushort patrol2 : 1;
	ushort jump_ahead : 1;
	ushort monkey_ahead : 1;
	mood_type mood;
	ITEM_INFO* enemy;
	ITEM_INFO ai_target;
	short pad;
	short item_num;
	PHD_VECTOR target;
	LOT_INFO LOT;
};

struct COLL_INFO
{
	long mid_floor;
	long mid_ceiling;
	long mid_type;
	long front_floor;
	long front_ceiling;
	long front_type;
	long left_floor;
	long left_ceiling;
	long left_type;
	long right_floor;
	long right_ceiling;
	long right_type;
	long left_floor2;
	long left_ceiling2;
	long left_type2;
	long right_floor2;
	long right_ceiling2;
	long right_type2;
	long radius;
	long bad_pos;
	long bad_neg;
	long bad_ceiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	short old_anim_state;
	short old_anim_number;
	short old_frame_number;
	short facing;
	short quadrant;
	short coll_type;
	short* trigger;
	char tilt_x;
	char tilt_z;
	char hit_by_baddie;
	char hit_static;
	ushort slopes_are_walls : 2;
	ushort slopes_are_pits : 1;
	ushort lava_is_pit : 1;
	ushort enable_baddie_push : 1;
	ushort enable_spaz : 1;
	ushort hit_ceiling : 1;
};

struct CAMERA_INFO
{
	GAME_VECTOR pos;
	GAME_VECTOR target;
	camera_type type;
	camera_type old_type;
	long shift;
	long flags;
	long fixed_camera;
	long number_frames;
	long bounce;
	long underwater;
	long target_distance;
	short target_angle;
	short target_elevation;
	short actual_elevation;
	short actual_angle;
	short lara_node;
	short box;
	short number;
	short last;
	short timer;
	short speed;
	short targetspeed;
	ITEM_INFO* item;
	ITEM_INFO* last_item;
	OBJECT_VECTOR* fixed;
	int mike_at_lara;
	PHD_VECTOR mike_pos;

	struct
	{
		float x;
		float y;
		float z;
	} fpos;

	struct
	{
		float x;
		float y;
		float z;
	} ftgt;
};

struct OBJECT_INFO
{
	short nmeshes;
	short mesh_index;
	long bone_index;
	short* frame_base;
	void (*initialise)(short item_number);
	void (*control)(short item_number);
	void (*floor)(ITEM_INFO* item, long x, long y, long z, long* height);
	void (*ceiling)(ITEM_INFO* item, long x, long y, long z, long* height);
	void (*draw_routine)(ITEM_INFO* item);
	void (*collision)(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll);
	short object_mip;
	short anim_index;
	short hit_points;
	short pivot_length;
	short radius;
	short shadow_size;
	ushort bite_offset;
	ushort loaded : 1;
	ushort intelligent : 1;
	ushort non_lot : 1;
	ushort save_position : 1;
	ushort save_hitpoints : 1;
	ushort save_flags : 1;
	ushort save_anim : 1;
	ushort semi_transparent : 1;
	ushort water_creature : 1;
	ushort using_drawanimating_item : 1;
	ushort HitEffect : 2;
	ushort undead : 1;
	ushort save_mesh : 1;
	void (*draw_routine_extra)(ITEM_INFO* item);
	ulong explodable_meshbits;
	ulong padfuck;
};

struct FOGBULB
{
	float px;
	float py;
	float pz;
	float rad;
	float sqrad;
	float den;
	float r; 
	float g;
	float b;
};

struct PCLIGHT_INFO
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	uchar Type;
	uchar Pad;
	short fuckpad;
};

struct ROOM_INFO
{
	short* data;
	short* door;
	FLOOR_INFO* floor;
	LIGHTINFO* light;
	MESH_INFO* mesh;
	long x;
	long y;
	long z;
	long minfloor;
	long maxceiling;
	short x_size;
	short y_size;
	long ambient;
	short num_lights;
	short num_meshes;
	uchar ReverbType;
	uchar FlipNumber;
	char MeshEffect;
	char bound_active;
	short left;
	short right;
	short top;
	short bottom;
	short test_left;
	short test_right;
	short test_top;
	short test_bottom;
	short item_number;
	short fx_number;
	short flipped_room;
	ushort flags;
	int nVerts;
	int nWaterVerts;
	int nShoreVerts;
	LPDIRECT3DVERTEXBUFFER SourceVB;
	short* FaceData;
	float posx;
	float posy;
	float posz;
	D3DVECTOR* vnormals;
	D3DVECTOR* fnormals;
	long* prelight;
	long* prelightwater;
	int	watercalc;
	D3DVECTOR* verts;
	int gt3cnt;
	int gt4cnt;
	PCLIGHT_INFO* pclight;
	FOGBULB* fogbulb;
	int nPCLight;
	int nFogBulbs;
	float cy0;
	float cy1;
	int nRoomlets;
	ROOMLET* pRoomlets;
	float* pRmVtx;
	short* pRmFace;
	int* pRmPrelight;
	int	vDumpSz;
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
};

struct STATS 
{
	ulong Timer;
	ulong Distance;
	ulong AmmoUsed;
	ulong AmmoHits;
	ushort Kills;
	uchar Secrets;
	uchar HealthUsed;
};

struct BLOOD_STRUCT
{
	long x;
	long y;
	long z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	uchar sSize;
	uchar dSize;
	uchar Size;
	uchar Friction;
	char RotAdd;
	uchar On;
	uchar sShade;
	uchar dShade;
	uchar Shade;
	uchar ColFadeSpeed;
	uchar FadeToBlack;
	char sLife;
	char Life;
	char Pad;
};

struct LARA_INFO
{
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short current_xvel;
	short current_yvel;
	short current_zvel;
	short spaz_effect_count;
	short flare_age;
	short BurnCount;
	short weapon_item;
	short back_gun;
	short flare_frame;
	short poisoned;
	short dpoisoned;
	uchar Anxiety;
	uchar wet[15];
	ushort flare_control_left : 1;
	ushort Unused1 : 1;
	ushort look : 1;
	ushort burn : 1;
	ushort keep_ducked : 1;
	ushort IsMoving : 1;
	ushort CanMonkeySwing : 1;
	ushort BurnBlue : 2;
	ushort Gassed : 1;
	ushort BurnSmoke : 1;
	ushort IsDucked : 1;
	ushort has_fired : 1;
	ushort Busy : 1;
	ushort LitTorch : 1;
	ushort IsClimbing : 1;
	ushort Fired : 1;
	long water_surface_dist;
	PHD_VECTOR last_pos;
	FX_INFO* spaz_effect;
	int mesh_effects;
	short* mesh_ptrs[15];
	ITEM_INFO* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	ushort holster;
	CREATURE_INFO* creature;
	long CornerX;
	long CornerZ;
	char RopeSegment;
	char RopeDirection;
	short RopeArcFront;
	short RopeArcBack;
	short RopeLastX;
	short RopeMaxXForward;
	short RopeMaxXBackward;
	long RopeDFrame;
	long RopeFrame;
	ushort RopeFrameRate;
	ushort RopeY;
	long RopePtr;
	void* GeneralPtr;
	int RopeOffset;
	ulong RopeDownVel;
	char RopeFlag;
	char MoveCount;
	int RopeCount;
	char skelebob;
	char pistols_type_carried;
	char uzis_type_carried;
	char shotgun_type_carried;
	char crossbow_type_carried;
	char hk_type_carried;
	char sixshooter_type_carried;
	char lasersight;
	char silencer;
	char binoculars;
	char crowbar;
	char examine1;
	char examine2;
	char examine3;
	char wetcloth;
	char bottle;
	char puzzleitems[12];
	ushort puzzleitemscombo;
	ushort keyitems;
	ushort keyitemscombo;
	ushort pickupitems;
	ushort pickupitemscombo;
	short num_small_medipack;
	short num_large_medipack;
	short num_flares;
	short num_pistols_ammo;
	short num_uzi_ammo;
	short num_revolver_ammo;
	short num_shotgun_ammo1;
	short num_shotgun_ammo2;
	short num_hk_ammo1;
	short num_crossbow_ammo1;
	short num_crossbow_ammo2;
	char location;
	char highest_location;
	char locationPad;
	uchar TightRopeOnCount;
	uchar TightRopeOff;
	uchar TightRopeFall;
	uchar ChaffTimer;
};

struct SAVEGAME_INFO//size=7656
{
	short Checksum;
	char things_to_figure_out[13];
	uchar AutoTarget;
	LARA_INFO Lara;
	STATS Level;
	STATS Game;
	short WeaponObject;
	short WeaponAnim;
	short WeaponFrame;
	short WeaponCurrent;
	short WeaponGoal;
	ulong CutSceneTriggered1;
	ulong CutSceneTriggered2;
	char GameComplete;
	uchar CurrentLevel;
	uchar CampaignSecrets[4];
	uchar TLCount;
	char buffer[7232];
};

struct OBJECT_TEXTURE_VERT
{
	float x;
	float y;
};

struct DYNAMIC
{
	long x;
	long y;
	long z;
	uchar on;
	uchar r;
	uchar g;
	uchar b;
	ushort falloff;
	uchar used;
	uchar pad1[1];
	long FalloffScale;
};

struct SMOKE_SPARKS
{
	long x;
	long y;
	long z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	short Flags;
	uchar sSize;
	uchar dSize;
	uchar Size;
	uchar Friction;
	uchar Scalar;
	uchar Def;
	char RotAdd;
	char MaxYvel;
	uchar On;
	uchar sShade;
	uchar dShade;
	uchar Shade;
	uchar ColFadeSpeed;
	uchar FadeToBlack;
	char sLife;
	char Life;
	uchar TransType;
	uchar FxObj;
	uchar NodeNumber;
	uchar mirror;
};

struct OBJLIST
{
	short invitem;
	ushort yrot;
	ushort bright;
};

struct RINGME
{
	OBJLIST current_object_list[100];
	int ringactive;
	int objlistmovement;
	int curobjinlist;
	int numobjectsinlist;
};

struct COMBINELIST
{
	void(*combine_routine)(int flag);
	short item1;
	short item2;
	short combined_item;
};

struct DOORPOS_DATA
{
	FLOOR_INFO* floor;
	FLOOR_INFO data;
	short block;
};

struct DOOR_DATA
{
	DOORPOS_DATA d1;
	DOORPOS_DATA d1flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2flip;
	short Opened;
	short* dptr1;
	short* dptr2;
	short* dptr3;
	short* dptr4;
	char dn1;
	char dn2;
	char dn3;
	char dn4;
	ITEM_INFO* item;
};

struct BOX_INFO
{
	uchar left;
	uchar right;
	uchar top;
	uchar bottom;
	short height;
	short overlap_index;
};

struct SPARKS
{
	long x;
	long y;
	long z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	short Flags;
	uchar sSize;
	uchar dSize;
	uchar Size;
	uchar Friction;
	uchar Scalar;
	uchar Def;
	char RotAdd;
	char MaxYvel;
	uchar On;
	uchar sR;
	uchar sG;
	uchar sB;
	uchar dR;
	uchar dG;
	uchar dB;
	uchar R;
	uchar G;
	uchar B;
	uchar ColFadeSpeed;
	uchar FadeToBlack;
	uchar sLife;
	uchar Life;
	uchar TransType;
	uchar extras;
	char Dynamic;
	uchar FxObj;
	uchar RoomNumber;
	uchar NodeNumber;
};

struct SHATTER_ITEM
{
	SPHERE Sphere;
	ITEM_LIGHT* il;
	short* meshp;
	long Bit;
	short YRot;
	short Flags;
};

struct CUTSEQ_ROUTINES
{
	void(*init_func)();
	void(*control_func)();
	void(*end_func)();
};

struct ACTORME
{
	int offset;
	short objslot;
	short nodes;
};

struct NEW_CUTSCENE
{
	short numactors;
	short numframes;
	int orgx;
	int orgy;
	int orgz;
	int audio_track;
	int camera_offset;
	ACTORME actor_data[10];
};

struct INVOBJ
{
	short object_number;
	short yoff;
	short scale1;
	short yrot;
	short xrot;
	short zrot;
	short flags;
	short objname;
	ulong meshbits;
};

struct BITE_INFO
{
	long x;
	long y;
	long z;
	long mesh_num;
};

struct TWOGUN_INFO
{
	PHD_3DPOS pos;
	short life;
	short coil;
	short spin;
	short spinadd;
	short length;
	short dlength;
	short size;
	char r;
	char g;
	char b;
	char fadein;
};

struct AMMOLIST
{
	short invitem;
	short amount;
	ushort yrot;
};

struct MENUTHANG
{
	int type;
	char* text;
};

struct VECTOR
{
	long vx;
	long vy;
	long vz;
};

struct AI_INFO
{
	short zone_number;
	short enemy_zone;
	long distance;
	long ahead;
	long bite;
	short angle;
	short x_angle;
	short enemy_facing;
};

struct RTDECODE
{
	ulong length;
	ulong off;
	ushort counter;
	ushort data;
	uchar decodetype;
	uchar packmethod;
	ushort padfuck;
};

struct PACKNODE
{
	short xrot_run;
	short yrot_run;
	short zrot_run;
	short xkey;
	short ykey;
	short zkey;
	RTDECODE decode_x;
	RTDECODE decode_y;
	RTDECODE decode_z;
	ulong xlength;
	ulong ylength;
	ulong zlength;
	char* xpacked;
	char* ypacked;
	char* zpacked;
};

struct NODELOADHEADER
{
	short xkey;
	short ykey;
	short zkey;
	short packmethod;
	short xlength;
	short ylength;
	short zlength;
};

struct STATIC_INFO
{
	short mesh_number;
	short flags;
	short x_minp;
	short x_maxp;
	short y_minp;
	short y_maxp;
	short z_minp;
	short z_maxp;
	short x_minc;
	short x_maxc;
	short y_minc;
	short y_maxc;
	short z_minc;
	short z_maxc;
};

struct ROPE_STRUCT
{
	PHD_VECTOR Segment[24];
	PHD_VECTOR Velocity[24];
	PHD_VECTOR NormalisedSegment[24];
	PHD_VECTOR MeshSegment[24];
	PHD_VECTOR Position;
	long Coords[24][3];
	long SegmentLength;
	short Active;
	short Coiled;
};

struct PENDULUM
{
	PHD_VECTOR Position;
	PHD_VECTOR Velocity;
	int node;
	ROPE_STRUCT* Rope;
};

struct MATRIX3D
{
	long m00, m01, m02, m03;
	long m10, m11, m12, m13;
	long m20, m21, m22, m23;
};

struct PCSVECTOR
{
	long vx;
	long vy;
	long vz;
};

struct ACMESHVERTEX
{
	float	x;
	float	y;
	float	z;
	float	nx;
	float	ny;
	float	nz;
	int		prelight;
	int		padd;
};

struct MESH_DATA
{
	short x;
	short y;
	short z;
	short r;
	short flags;
	short nVerts;
	short nNorms;
	short ngt4;
	short* gt4;
	short ngt3;
	short* gt3;
	long* prelight;
	LPDIRECT3DVERTEXBUFFER SourceVB;
	D3DVECTOR* Normals;
	int	aFlags;
	ACMESHVERTEX* aVtx;
	float bbox[6];
};

struct TEXTURESTRUCT
{
	ushort drawtype;
	ushort tpage;
	ushort flag;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;
	float u4;
	float v4;
};

struct PHDTEXTURESTRUCT
{
	ushort drawtype;
	ushort tpage;
	ushort flag;
	ushort u1;
	ushort v1;
	ushort u2;
	ushort v2;
	ushort u3;
	ushort v3;
	ushort u4;
	ushort v4;
	ushort padd;
	ulong xoff;
	ulong yoff;
	ulong width;
	ulong height;
};

typedef struct _ENVUV
{
	float	u, v;
}ENVUV, *LPENVUV;

struct AFRVECTOR
{
	float	vx;
	float	vy;
	float	vz;
};

struct DXDISPLAYMODE
{
	int w;
	int h;
	int bpp;
	long RefreshRate;
	int bPalette;
	DDSURFACEDESC2	ddsd;
	uchar	rbpp;
	uchar	gbpp;
	uchar	bbpp;
	uchar	rshift;
	uchar	gshift;
	uchar	bshift;

};

struct DXTEXTUREINFO
{
	DDPIXELFORMAT ddpf;
	ulong bpp;
	int bPalette;
	int bAlpha;
	uchar rbpp;
	uchar gbpp;
	uchar bbpp;
	uchar abpp;
	uchar rshift;
	uchar gshift;
	uchar bshift;
	uchar ashift;
};

struct DXZBUFFERINFO
{
	DDPIXELFORMAT ddpf;
	ulong bpp;
};

struct DXD3DDEVICE
{
	char Name[30];
	char About[80];
	LPGUID lpGuid;
	GUID Guid;
	D3DDEVICEDESC DeviceDesc;
	int bHardware;
	int nDisplayModes;
	DXDISPLAYMODE* DisplayModes;
	int nTextureInfos;
	DXTEXTUREINFO* TextureInfos;
	int nZBufferInfos;
	DXZBUFFERINFO* ZBufferInfos;
};

struct DXDIRECTDRAWINFO
{
	char Name[30];  
	char About[80];
	LPGUID lpGuid;
	GUID Guid;
	DDCAPS DDCaps;
	DDDEVICEIDENTIFIER DDIdentifier;
	int nDisplayModes;
	DXDISPLAYMODE* DisplayModes;
	int nD3DDevices;
	DXD3DDEVICE* D3DDevices;
};

struct DXDIRECTSOUNDINFO
{
	char Name[30];
	char About[80];
	LPGUID lpGuid;
	GUID Guid;
};

struct DXINFO
{
	int nDDInfo;
	int nDSInfo;
	DXDIRECTDRAWINFO* DDInfo;
	DXDIRECTSOUNDINFO* DSInfo;
	int nDD;
	int nD3D;
	int nDisplayMode;
	int nTexture;
	int nZBuffer;
	int nDS;
	bool bHardware;
};

struct DXPTR
{
	LPDIRECTDRAW4 lpDD;
	LPDIRECT3D3 lpD3D;
	LPDIRECT3DDEVICE3 lpD3DDevice;
	LPDIRECT3DDEVICE3 _lpD3DDevice;
	LPDIRECTDRAWSURFACE4 lpPrimaryBuffer;
	LPDIRECTDRAWSURFACE4 lpBackBuffer;
	LPDIRECTDRAWSURFACE4 lpZBuffer;
	LPDIRECT3DVIEWPORT3 lpViewport;
	LPDIRECTSOUND lpDS;
	ulong dwRenderWidth;
	ulong dwRenderHeight;
	RECT rViewport;
	RECT rScreen;
	int Flags;
	int WindowStyle;
	long CoopLevel;
	IDirectInput7* lpDirectInput;
	IDirectInputDevice7* Keyboard;
	IDirectInputDevice7* Joystick;
	HWND hWnd;
	volatile int InScene;
	volatile int WaitAtBeginScene;
	volatile int DoneBlit;
};

struct WINAPP
{
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASS WindowClass;
	DXINFO DXInfo;
	DXPTR dx;
	HANDLE mutex;
	float fps;
	LPDIRECT3DMATERIAL3 GlobalMaterial;
	D3DMATERIALHANDLE GlobalMaterialHandle;
	HACCEL hAccel;
	bool SetupComplete;
	bool BumpMapping;
	int TextureSize;
	int BumpMapSize;
	bool mmx;
	bool Filtering;
	bool Volumetric;
	bool SoundDisabled;
	long StartFlags;
	volatile bool fmv;
	long Desktopbpp;
	long AutoTarget;
};

struct FIRE_LIST
{
	long x;
	long y;
	long z;
	char on;
	char size;
	short room_number;
};

struct FIRE_SPARKS
{
	short x;
	short y;
	short z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	short Flags;
	uchar sSize;
	uchar dSize;
	uchar Size;
	uchar Friction;
	uchar Scalar;
	uchar Def;
	char RotAdd;
	char MaxYvel;
	uchar On;
	uchar sR;
	uchar sG;
	uchar sB;
	uchar dR;
	uchar dG;
	uchar dB;
	uchar R;
	uchar G;
	uchar B;
	uchar ColFadeSpeed;
	uchar FadeToBlack;
	uchar sLife;
	uchar Life;
};

struct DEBRIS_STRUCT
{
	void* TextInfo;
	long x;
	long y; 
	long z;
	short XYZOffsets1[3];
	short Dir;
	short XYZOffsets2[3];
	short Speed;
	short XYZOffsets3[3];
	short Yvel;
	short Gravity;
	short RoomNumber;
	uchar On;
	uchar XRot;
	uchar YRot;
	uchar r;
	uchar g;
	uchar b;
	uchar Pad[2];
	long color1;
	long color2;
	long color3;
	long ambient;
	long flags;
};

struct GUNSHELL_STRUCT
{
	PHD_3DPOS pos;
	short fallspeed;
	short room_number;
	short speed;
	short counter;
	short DirXrot;
	short object_number;
};

struct BUBBLE_STRUCT 
{
	PHD_VECTOR pos;
	short room_number;
	short speed;
	short size;
	short dsize;
	uchar shade;
	uchar vel;
	uchar y_rot;
	char Flags;
	short Xvel;
	short Yvel;
	short Zvel;
	short Pad;
};

struct GUNFLASH_STRUCT
{
	MATRIX3D matrix;
	short on;
};

struct DRIP_STRUCT
{
	long x;
	long y;
	long z;
	int lnode;
	int ox;
	int oy;
	int oz;
	uchar On;
	uchar R;
	uchar G;
	uchar B;
	short Yvel;
	uchar Gravity;
	uchar Life;
	short RoomNumber;
	uchar Outside;
	uchar Pad;
};

struct SHOCKWAVE_STRUCT
{
	long x;
	long y;
	long z;
	short InnerRad;
	short OuterRad;
	short XRot;
	short Flags;
	uchar r;
	uchar g;
	uchar b;
	uchar life;
	short Speed;
	short Temp;
};

struct SPLASH_STRUCT
{
	long x;
	long y;
	long z;
	short InnerRad;
	short InnerSize;
	short InnerRadVel;
	short InnerYVel;
	short InnerY;
	short MiddleRad;
	short MiddleSize;
	short MiddleRadVel;
	short MiddleYVel;
	short MiddleY;
	short OuterRad;
	short OuterSize;
	short OuterRadVel;
	char flags;
	uchar life;
};

struct SPLASH_SETUP
{
	long x;
	long y;
	long z;
	short InnerRad;
	short InnerSize;
	short InnerRadVel;
	short InnerYVel;
	short pad1;
	short MiddleRad;
	short MiddleSize;
	short MiddleRadVel;
	short MiddleYVel;
	short pad2;
	short OuterRad;
	short OuterSize;
	short OuterRadVel;
	short pad3;
};

struct RIPPLE_STRUCT
{
	long x;
	long y;
	long z;
	char flags;
	uchar life;
	uchar size;
	uchar init;
};

struct LASER_STRUCT
{
	SVECTOR v1[3];
	SVECTOR v2[3];
	SVECTOR v3[3];
	SVECTOR v4[3];
	short Rand[18];
};

struct STEAMLASER_STRUCT
{
	SVECTOR v1[2];
	SVECTOR v2[2];
	SVECTOR v3[2];
	SVECTOR v4[2];
	short Rand[27];
};

struct FLOORLASER_STRUCT
{
	SVECTOR v1;
	SVECTOR v2;
	SVECTOR v3;
	SVECTOR v4;
	short Rand[121];
	short Pulse[121];
};

struct SPOTCAM
{
	long x;
	long y;
	long z;
	long tx;
	long ty;
	long tz;
	uchar sequence;
	uchar camera;
	short fov;
	short roll;
	short timer;
	short speed;
	short flags;
	short room_number;
	short pad;
};

struct SAVEGAMES
{
	char SaveName[75];
	char bValid;
	short Hour;
	short Min;
	short Sec;
	short Day;
	long Count;
};

struct PHDSPRITESTRUCT
{
	ushort tpage;
	ushort offset;
	ushort width;
	ushort height;
	short x1;
	short y1;
	short x2;
	short y2;
};

struct SPRITESTRUCT
{
	ushort tpage;
	ushort offset;
	ushort width;
	ushort height;
	float	x1;
	float	y1;
	float	x2;
	float	y2;
};

struct RAT_STRUCT
{
	PHD_3DPOS pos;
	short room_number;
	short speed;
	short fallspeed;
	uchar On;
	uchar flags;
};

struct BAT_STRUCT
{
	PHD_3DPOS pos;
	short room_number;
	short speed;
	short Counter;
	short LaraTarget;
	char XTarget;
	char ZTarget;
	uchar On;
	uchar flags;
};

struct QUAKE_CAM
{
	GAME_VECTOR spos;
	GAME_VECTOR epos;
};

struct LIGHTNING_STRUCT
{
	PHD_VECTOR Point[4];
	uchar r;
	uchar g;
	uchar b;
	uchar Life;
	char Xvel1;
	char Yvel1;
	char Zvel1;
	char Xvel2;
	char Yvel2;
	char Zvel2;
	char Xvel3;
	char Yvel3;
	char Zvel3;
	uchar Size;
	uchar Flags;
	uchar Rand;
	uchar Segments;
	uchar Pad[3];
};

struct SPIDER_STRUCT
{
	PHD_3DPOS pos;
	short room_number;
	short speed;
	short fallspeed;
	uchar On;
	uchar flags;
};

struct SNOWFLAKE
{
	long x;
	long y;
	long z;
	char xv;
	uchar yv;
	char zv;
	uchar life;
	short stopped;
	short room_number;
};

struct PORTAL_STRUCT
{
	SVECTOR v1;
	SVECTOR v2;
	SVECTOR v3;
	SVECTOR v4;
	short Rand[64];
	CVECTOR orgb[64];
	CVECTOR rgb[64];
};
#pragma pack(pop)
