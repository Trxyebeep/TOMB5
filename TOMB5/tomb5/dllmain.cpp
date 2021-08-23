#include "../tomb5/pch.h"
#include "../game/lara.h"
#include "../game/collide.h"
#include "../game/deltapak.h"
#include "../game/control.h"
#include "../game/tomb4fx.h"
#include "../game/pickup.h"
#include "../game/camera.h"
#include "../game/laramisc.h"
#include "../game/items.h"
#include "../game/draw.h"
#include "../game/gameflow.h"
#include "../game/newinv2.h"
#include "../game/objects.h"
#include "../game/laraflar.h"
#include "../game/effects.h"
#include "../game/larafire.h"
#include "../game/effect2.h"
#include "../game/lara1gun.h"
#include "../game/hair.h"
#include "../specific/input.h"
#include "../game/laraswim.h"
#include "../game/larasurf.h"
#include "../game/door.h"
#include "../game/laraclmb.h"
#include "../game/health.h"
#include "../game/spotcam.h"
#include "../game/cutseq.h"
#include "../game/chef.h"
#include "../game/people.h"
#include "../specific/function_stubs.h"
#include "../game/joby.h"
#include "../specific/output.h"
#include "../specific/3dmath.h"
#include "../game/rope.h"
#include "../specific/LoadSave.h"
#include "../specific/drawlara.h"
#include "../game/laraskin.h"
#include "../game/subsuit.h"
#include "../specific/function_table.h"
#include "../game/traps.h"
#include "../game/init.h"
#include "../specific/file.h"
#include "../specific/alexstuff.h"
#include "../game/objlight.h"
#include "../game/swampy.h"
#include "../game/rat.h"
#include "../game/deathsld.h"
#include "../game/footprnt.h"
#include "../game/bat.h"
#include "../game/dog.h"
#include "../game/huskie.h"
#include "../specific/specificfx.h"
#include "../game/skeleton.h"
#include "../game/spider.h"
#include "../game/hydra.h"
#include "../game/imp.h"
#include "../game/willwisp.h"
#include "../game/sniper.h"
#include "../game/minisub.h"
#include "../game/autogun.h"
#include "../game/twogun.h"
#include "../game/tower2.h"
#include "../game/andy.h"
#include "../game/setup.h"
#include "../game/switch.h"
#include "../game/andrea.h"
#include "../game/mafia2.h"
#include "../game/lion.h"
#include "../game/guardian.h"
#include "../game/box.h"
#include "../game/crow.h"

HINSTANCE hinstance = NULL;

#ifndef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT int dummyz();

int dummyz()
{
	return 0;
}

void inject_all(bool replace)
{
	inject_lara(replace);
	inject_coll(replace);
	inject_deltaPak(replace);
	inject_control(replace);
	inject_tomb4fx(replace);
	inject_pickup(replace);
	inject_camera(replace);
	inject_laramisc(replace);
	inject_items(replace);
	inject_draw(replace);
	inject_gameflow(replace);
	inject_newinv2(replace);
	inject_objects(replace);
	inject_laraflar(replace);
	inject_effects(replace);
	inject_larafire(replace);
	inject_lara1gun(replace);
	inject_hair(replace);
	inject_input(replace);
	inject_laraswim(replace);
	inject_larasurf(replace);
	inject_door(replace);
	inject_laraclmb(replace);
	inject_health(replace);
	inject_effect2(replace);
	inject_spotcam(replace);
	inject_cutseq(replace);
	inject_chef(replace);
	inject_people(replace);
	inject_funcStubs(replace);
	inject_joby(replace);
	inject_3dmath(replace);
	inject_output(replace);
	inject_rope(replace);
	inject_LoadSave(replace);
	inject_drawlara(replace);
	inject_laraskin(replace);
	inject_subsuit(replace);
	inject_functbl(replace);
	inject_traps(replace);
	inject_init(replace);
	inject_file(replace);
	inject_alexstuff(replace);
	inject_objlight(replace);
	inject_swampy(replace);
	inject_rat(replace);
	inject_deathslide(replace);
	inject_footprnt(replace);
	inject_bats(replace);
	inject_dog(replace);
	inject_huskie(replace);
	inject_specificfx(replace);
	inject_skelly(replace);
	inject_spider(replace);
	inject_hydra(replace);
	inject_imp(replace);
	inject_wisp(replace);
	inject_sniper(replace);
	inject_minisub(replace);
	inject_autogun(replace);
	inject_twogun(replace);
	inject_tower2(replace);
	inject_andy(replace);
	inject_setup(replace);
	inject_switch(replace);
	inject_andrea(replace);
	inject_mafia(replace);
	inject_lion(replace);
	inject_guardian(replace);
	inject_box(replace);
	inject_crow(replace);
}

BOOL APIENTRY DllMain(HINSTANCE hmodule, DWORD ul_reason_for_call, LPVOID lpreserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hinstance = hmodule;
		inject_all(true);

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return 1;
}
