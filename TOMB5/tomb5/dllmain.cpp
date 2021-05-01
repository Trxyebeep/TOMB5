// dllmain.cpp : Defines the entry point for the DLL application.
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
#include "../specific/specific.h"
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
#include "../specific/init.h"
#include "../game/joby.h"
#include "../specific/output.h"
#include "../specific/3dmath.h"

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

void inject_all()
{
    inject_lara();
    inject_coll();
    inject_deltaPak();
    inject_control();
    inject_tomb4fx();
    inject_pickup();
    inject_camera();
    inject_laramisc();
    inject_items();
    inject_draw();
    inject_gameflow();
    inject_newinv2();
    inject_objects();
    inject_laraflar();
    inject_effects();
    inject_specific();
    inject_larafire();
    inject_lara1gun();
    inject_hair();
    inject_input();
    inject_laraswim();
    inject_larasurf();
    inject_door();
    inject_laraclmb();
    inject_health();
    inject_effect2();
    inject_spotcam();
    inject_cutseq();
    inject_chef();
    inject_people();
    inject_init();
    inject_joby();
    inject_3dmath();
    inject_output();
}

BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hinstance = hModule;
        inject_all();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

