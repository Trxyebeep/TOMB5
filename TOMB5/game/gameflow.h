#pragma once
#include "../global/vars.h"


void inject_gameflow(bool replace);

void DoGameflow();
long TitleOptions();
void DoTitle(uchar name, uchar audio);
long do_dels_cutseq_selector();
void DoLevel(uchar name, uchar audio);
void LoadGameflow();

extern GAMEFLOW* Gameflow;
extern PHD_VECTOR gfLensFlare;
extern CVECTOR gfLayer1Col;
extern CVECTOR gfLayer2Col;
extern CVECTOR gfLensFlareColour;
extern CVECTOR gfFog;
extern ushort* gfStringOffset;
extern ushort* gfFilenameOffset;
extern uchar* gfScriptFile;
extern uchar* gfLanguageFile;
extern char* gfStringWad;
extern char* gfFilenameWad;
extern long gfStatus;
extern long gfMirrorZPlane;
extern short gfLevelFlags;
extern uchar gfCurrentLevel;
extern uchar gfLevelComplete;
extern uchar gfGameMode;
extern uchar gfRequiredStartPos;
extern uchar gfMirrorRoom;
extern uchar gfInitialiseGame;
extern uchar gfNumPickups;
extern uchar gfNumTakeaways;
extern uchar gfNumMips;
extern uchar gfPickups[16];
extern uchar gfTakeaways[16];
extern uchar gfMips[8];
extern uchar gfResidentCut[4];
extern uchar gfLevelNames[40];
extern char gfUVRotate;
extern char gfLayer1Vel;
extern char gfLayer2Vel;
extern ulong GameTimer;
extern long GlobalSoftReset;
extern uchar bDoCredits;
extern char JustLoaded;

enum gf_languages
{
	LNG_ENGLISH,
	LNG_FRENCH,
	LNG_GERMAN,
	LNG_ITALIAN,
	LNG_SPANISH,
	LNG_US,
	LNG_JAPAN,
	LNG_DUTCH,
	NUM_GF_LANGUAGES
};

enum gf_commands
{
	CMD_FMV = 0x80,
	CMD_LEVEL,
	CMD_TITLE,
	CMD_ENDSEQ,
	CMD_PLAYCUT,
	CMD_CUT1,
	CMD_CUT2,
	CMD_CUT3,
	CMD_CUT4,
	CMD_LAYER1,
	CMD_LAYER2,
	CMD_UVROT,
	CMD_LEGEND,
	CMD_LENSFLARE,
	CMD_MIRROR,
	CMD_FOG,
	CMD_ANIMATINGMIP,
	CMD_RESETHUB,

	CMD_KEY1,
	CMD_KEY2,
	CMD_KEY3,
	CMD_KEY4,
	CMD_KEY5,
	CMD_KEY6,
	CMD_KEY7,
	CMD_KEY8,
	CMD_KEY9,
	CMD_KEY10,
	CMD_KEY11,
	CMD_KEY12,

	CMD_PUZZLE1,
	CMD_PUZZLE2,
	CMD_PUZZLE3,
	CMD_PUZZLE4,
	CMD_PUZZLE5,
	CMD_PUZZLE6,
	CMD_PUZZLE7,
	CMD_PUZZLE8,
	CMD_PUZZLE9,
	CMD_PUZZLE10,
	CMD_PUZZLE11,
	CMD_PUZZLE12,

	CMD_PICKUP1,
	CMD_PICKUP2,
	CMD_PICKUP3,
	CMD_PICKUP4,

	CMD_EXAMINE1,
	CMD_EXAMINE2,
	CMD_EXAMINE3,

	CMD_KEYCOMBO1_1,
	CMD_KEYCOMBO1_2,
	CMD_KEYCOMBO2_1,
	CMD_KEYCOMBO2_2,
	CMD_KEYCOMBO3_1,
	CMD_KEYCOMBO3_2,
	CMD_KEYCOMBO4_1,
	CMD_KEYCOMBO4_2,
	CMD_KEYCOMBO5_1,
	CMD_KEYCOMBO5_2,
	CMD_KEYCOMBO6_1,
	CMD_KEYCOMBO6_2,
	CMD_KEYCOMBO7_1,
	CMD_KEYCOMBO7_2,
	CMD_KEYCOMBO8_1,
	CMD_KEYCOMBO8_2,

	CMD_PUZZLECOMBO1_1,
	CMD_PUZZLECOMBO1_2,
	CMD_PUZZLECOMBO2_1,
	CMD_PUZZLECOMBO2_2,
	CMD_PUZZLECOMBO3_1,
	CMD_PUZZLECOMBO3_2,
	CMD_PUZZLECOMBO4_1,
	CMD_PUZZLECOMBO4_2,
	CMD_PUZZLECOMBO5_1,
	CMD_PUZZLECOMBO5_2,
	CMD_PUZZLECOMBO6_1,
	CMD_PUZZLECOMBO6_2,
	CMD_PUZZLECOMBO7_1,
	CMD_PUZZLECOMBO7_2,
	CMD_PUZZLECOMBO8_1,
	CMD_PUZZLECOMBO8_2,

	CMD_PICKUPCOMBO1_1,
	CMD_PICKUPCOMBO1_2,
	CMD_PICKUPCOMBO2_1,
	CMD_PICKUPCOMBO2_2,
	CMD_PICKUPCOMBO3_1,
	CMD_PICKUPCOMBO3_2,
	CMD_PICKUPCOMBO4_1,
	CMD_PICKUPCOMBO4_2,

	CMD_GIVEOBJ,
	CMD_TAKEOBJ,
};

enum gf_level_options
{
	GF_YOUNGLARA = (1 << 0),
	GF_WEATHER = (1 << 1),
	GF_HORIZON = (1 << 2),
	GF_LAYER1 = (1 << 3),
	GF_LAYER2 = (1 << 4),
	GF_STARFIELD = (1 << 5),
	GF_LIGHTNING = (1 << 6),
	GF_OFFICE = (1 << 7),
	GF_PULSE = (1 << 8),
	GF_HORIZONCOLADD = (1 << 9),
	GF_RESETHUB = (1 << 10),
	GF_LENSFLARE = (1 << 11),
	GF_TIMER = (1 << 12),
	GF_MIRROR = (1 << 13),
	GF_REMOVEAMULET = (1 << 14),
	GF_NOLEVEL = (1 << 15)
};

enum gf_tr5_levels
{
	LVL5_TITLE,
	LVL5_STREETS_OF_ROME,
	LVL5_TRAJAN_MARKETS,
	LVL5_COLOSSEUM,
	LVL5_BASE,
	LVL5_SUBMARINE,
	LVL5_DEEPSEA_DIVE,
	LVL5_SINKING_SUBMARINE,
	LVL5_GALLOWS_TREE,
	LVL5_LABYRINTH,
	LVL5_OLD_MILL,
	LVL5_THIRTEENTH_FLOOR,
	LVL5_ESCAPE_WITH_THE_IRIS,
	LVL5_SECURITY_BREACH,
	LVL5_RED_ALERT,
	LVL5_GIBBY_LEVEL,
	LVL5_DEL_LEVEL,
	LVL5_TOM_LEVEL,

	NUM_TR5_LEVELS
};

enum gf_strings	//auto generated from the script compiler
{
	TXT_cack,
	TXT_Gibbys_Level,
	TXT_Dels_Level,
	TXT_Toms_Level,
	TXT_The_13th_floor,
	TXT_Escape_with_the_iris,
	TXT_Security_breach,
	TXT_Red_alert,
	TXT_The_base,
	TXT_The_submarine,
	TXT_Deepsea_dive,
	TXT_Sinking_submarine,
	TXT_Streets_of_Rome,
	TXT_Trajans_markets,
	TXT_The_Colosseum,
	TXT_Gallows_tree,
	TXT_Labyrinth,
	TXT_Old_mill,
	TXT_cut0,
	TXT_cut1,
	TXT_cut2,
	TXT_cut3,
	TXT_cut4,
	TXT_cut5,
	TXT_cut6,
	TXT_cut7,
	TXT_cut8,
	TXT_cut9,
	TXT_cut10,
	TXT_cut11,
	TXT_cut12,
	TXT_cut13,
	TXT_cut14,
	TXT_cut15,
	TXT_cut16,
	TXT_cut17,
	TXT_cut18,
	TXT_cut19,
	TXT_cut20,
	TXT_cut21,
	TXT_cut22,
	TXT_cut23,
	TXT_cut24,
	TXT_cut25,
	TXT_cut26,
	TXT_cut27,
	TXT_cut28,
	TXT_cut29,
	TXT_cut30,
	TXT_cut31,
	TXT_cut32,
	TXT_cut33,
	TXT_cut34,
	TXT_cut35,
	TXT_cut36,
	TXT_keypad,
	TXT_Unlimited_s,
	TXT_USE,
	TXT_CHOOSE_AMMO,
	TXT_CHOOSE_WEAPON_MODE,
	TXT_COMBINE,
	TXT_SEPERATE,
	TXT_EQUIP,
	TXT_COMBINE_WITH,
	TXT_LOAD_GAME,
	TXT_SAVE_GAME,
	TXT_EXAMINE,
	TXT_More,
	TXT_Select_Ammo,
	TXT_Select_Option,
	TXT_Combine,
	TXT_Push_Keypad,
	TXT_Movement,
	TXT_STORYMODE,
	TXT_GRAP1,
	TXT_GRAP2,
	TXT_Wet_Cloth,
	TXT_Cloth,
	TXT_Bottle,
	TXT_Uzi,
	TXT_Pistols,
	TXT_Shotgun,
	TXT_Revolver,
	TXT_Revolver_LaserSight,
	TXT_DESERT1,
	TXT_DESERT2,
	TXT_DESERT3,
	TXT_HKSTRING0,
	TXT_HKSTRING1,
	TXT_Shotgun_Normal_Ammo,
	TXT_Shotgun_Wideshot_Ammo,
	TXT_HKSTRING2,
	TXT_HKSTRING3,
	TXT_HKSTRING4,
	TXT_HKSTRING5,
	TXT_Revolver_Ammo,
	TXT_Uzi_Ammo,
	TXT_Pistol_Ammo,
	TXT_LaserSight,
	TXT_Silencer,
	TXT_Large_Medipack,
	TXT_Small_Medipack,
	TXT_Binoculars,
	TXT_Headset,
	TXT_Flares,
	TXT_TMX,
	TXT_Load,
	TXT_Save,
	TXT_Crowbar,
	TXT_Garden_Key,
	TXT_Saturn_Symbol,
	TXT_Golden_Key_1,
	TXT_Golden_Key_2,
	TXT_Mercury_stone,
	TXT_Teleporter_disc,
	TXT_A_Fuse,
	TXT_Silver_key,
	TXT_Bronze_key,
	TXT_Swipe_Card,
	TXT_Hammer,
	TXT_Access_code_disc,
	TXT_Iris_lab_access,
	TXT_High_level_access_card,
	TXT_ID_access_card,
	TXT_Iris_artifact,
	TXT_Low_level_access_card,
	TXT_Restroom_access_card,
	TXT_Helipad_access_key,
	TXT_Key_bit_left,
	TXT_Key_bit_right,
	TXT_Valve_wheel,
	TXT_Golden_coin,
	TXT_Mars_symbol,
	TXT_Venus_symbol,
	TXT_Gemstone,
	TXT_Philosophers_stone,
	TXT_Colosseum_key_1,
	TXT_Colosseum_key_2,
	TXT_Gemstone_piece,
	TXT_Spear_Of_Destiny,
	TXT_Heart,
	TXT_Catapult,
	TXT_Iron_clapper,
	TXT_Rubber_tube,
	TXT_Pitchfork,
	TXT_Bone_dust,
	TXT_Bestiary,
	TXT_Chalk,
	TXT_Silver_coin,
	TXT_Suit_console,
	TXT_Suit_battery,
	TXT_Aqualung,
	TXT_Suit_console2,
	TXT_Battery_,
	TXT_Battery_2,
	TXT_Chaff_flares,
	TXT_Nitrogen_canister,
	TXT_Oxygen_canister,
	TXT_Yes,
	TXT_No,
	TXT_Load_Game,
	TXT_New_Game,
	TXT_Paused,
	TXT_Select_Game_To_Load,
	TXT_Select_Game_To_Save,
	TXT_Special_Features,
	TXT_Movie_Trailer,
	TXT_Storyboards_Part_1,
	TXT_Next_Generation_Concept,
	TXT_Storyboards_Part_2,
	TXT_Next_Generation_Preview,
	TXT_GAME_OVER,
	TXT_Save_Game,
	TXT_Exit_to_Title,
	TXT_DEMO_MODE,
	TXT_Current_Location,
	TXT_Statistics,
	TXT_Distance_Travelled,
	TXT_Ammo_Used,
	TXT_Secrets_Found,
	TXT_Location,
	TXT_Health_Packs_Used,
	TXT_Time_Taken,
	TXT_days,
	TXT_of,
	TXT_DELSECRET,
	TXT_Action,
	TXT_Draw_Weapon,
	TXT_Jump,
	TXT_Roll,
	TXT_Walk,
	TXT_Look,
	TXT_Duck,
	TXT_Dash,
	TXT_Inventory,
	TXT_BSELECT,
	TXT_BCANCEL,
	TXT_BGOBACK,
	TXT_BOK,
	TXT_CANCELB,
	TXT_MEMCARD,
	TXT_INSERT,
	TXT_CHECKING,
	TXT_FORMAT,
	TXT_LOADING,
	TXT_SAVING,
	TXT_FORMATTING,
	TXT_OVERWRITE,
	TXT_UNFORMATTED,
	TXT_NOSPACE,
	TXT_NOGAMES,
	TXT_LOADING2,
	TXT_Load_OK,
	TXT_Saved_OK,
	TXT_Format_OK,
	TXT_Load_Failed,
	TXT_Save_Failed,
	TXT_Format_Failed,
	TXT_Empty,
	TXT_Controller_Removed,
	TXT_Resume,
	TXT_Quit,
	TXT_Game_Settings,
	TXT_Control_Options,
	TXT_Are_you_sure_you_want_to,
	TXT_Screen_Adjust,
	TXT_FXVOL,
	TXT_MUSICVOL,
	TXT_Use_directional_buttons,
	TXT_Configure_Controls,
	TXT_Configuration_n,
	TXT_Vibration_On,
	TXT_Vibration_Off,
	TXT_Manual_Targeting,
	TXT_Automatic_Targeting,
	TXT_PAD,
	TXT_PAD2,
	TXT_Programmers,
	TXT_Additional_Programmers,
	TXT_AI_Programming,
	TXT_Animators,
	TXT_Level_Designers,
	TXT_FMV_Sequences,
	TXT_Music_Sound_FX,
	TXT_Original_Story,
	TXT_Script,
	TXT_Producer,
	TXT_QA,
	TXT_Executive_Producers,
	TXT_Additional_Sound_FX,
	TXT_Rubbish,
	TXT_PC_Programmer,
	TXT_Programmers2,
	TXT_Additional_Programming,
	TXT_Resume2,
	TXT_Options,
	TXT_Empty_Slot,
	TXT_Music_Volume,
	TXT_SFX_Volume,
	TXT_Sound_Quality,
	TXT_Low,
	TXT_Medium,
	TXT_High,
	TXT_Targeting,
	TXT_Manual,
	TXT_Automatic,
	TXT_Graphics_Adapter,
	TXT_Output_Settings,
	TXT_Output_Resolution,
	TXT_Texture_Bit_Depth,
	TXT_OK,
	TXT_Cancel,
	TXT_Hardware_Acceleration,
	TXT_Software_Mode,
	TXT_Volumetric_FX,
	TXT_Bilinear_Filtering,
	TXT_Bump_Mapping,
	TXT_Low_Resolution_Textures,
	TXT_Low_Resolution_Bump_Maps,
	TXT_Sound_Device,
	TXT_Disable,
	TXT_Bit,
	TXT_Full_Screen,
	TXT_Windowed,
	TXT_Render_Options,
	TXT_No_Sound_Card_Installed,
	TXT_Failed_To_Setup_DirectX,
	TXT_Exit,
	TXT_Control_Configuration,
	TXT_Use_Flare,
	TXT_Control_Method,
	TXT_Mouse,
	TXT_Keyboard,
	TXT_Joystick,
	TXT_Waiting,
	TXT_Reset,
	TXT_Save_Settings,
	TXT_Saved_OK2,
	TXT_Step_Left,
	TXT_Step_Right,
	TXT_Play,
	TXT_Setup,
	TXT_Install,
	TXT_Uninstall,
	TXT_ReadMe,
	TXT_Demo_Mode_Esc_To_Exit,
	TXT_Select_Level,
	TXT_No_FMV,
	TXT_THE_LAST_STRING_ENTRY,
	TXT_NUM_STRINGS
};
