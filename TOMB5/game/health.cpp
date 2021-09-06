#include "../tomb5/pch.h"
#include "health.h"
#include "gameflow.h"
#include "newinv2.h"
#include "objects.h"
#include "../specific/output.h"
#include "../specific/LoadSave.h"
#include "text.h"
#ifdef AMMO_COUNTER
#include "larafire.h"
#endif

int FlashIt()
{
	static long flash_state;
	static long flash_count;

	if (flash_count)
		flash_count--;
	else
	{
		flash_state ^= 1;
		flash_count = 5;
	}

	return flash_state;
}

void DrawGameInfo(int timed)
{
	long flash_state, seconds;
	char buf[80];
#ifdef AMMO_COUNTER
	short ammo, btm;
#endif

	if (!GLOBAL_playing_cutseq && !bDisableLaraControl && gfGameMode != 1)
	{
		flash_state = FlashIt();
		DrawHealthBar(flash_state);
		DrawAirBar(flash_state);
		DrawPickups(timed);

		if (DashTimer < 120)
			S_DrawDashBar(100 * DashTimer / 120);

		if (gfLevelFlags & GF_TIMER && savegame.Level.Timer != 0 && savegame.Level.Timer < 0x1A5E0)
		{
			seconds = savegame.Level.Timer / 30;
			sprintf(&buf[0], "%.2d", seconds / 60);
			PrintString(0x28, 0x18, 0, &buf[0], 0);
			PrintString(0x3C, 0x18, 0, ":", 0);
			sprintf(&buf[0], "%.2d", seconds % 60);
			PrintString(0x42, 0x18, 0, &buf[0], 0);
			PrintString(0x56, 0x18, 0, ":", 0);
			sprintf(&buf[0], "%.2d", (338 * (savegame.Level.Timer % 30)) / 100);
			PrintString(0x5C, 0x18, 0, &buf[0], 0);
		}

#ifdef AMMO_COUNTER
		if (lara.gun_status == LG_READY)
		{
			ammo = *get_current_ammo_pointer(lara.gun_type);

			if (ammo == -1)
				return;

			if (lara.gun_type == WEAPON_SHOTGUN)
				ammo /= 6;
			
			sprintf(&buf[0], "%i", ammo);
			GetStringLength(buf, 0, &btm);
			PrintString(LaserSight ? phd_centerx + 30 : (phd_winxmax - GetStringLength(buf, 0, 0) - 80), phd_winymax - btm - 70, 0, &buf[0], 0);
		}
#endif
	}
}

void DrawHealthBar(int flash_state)
{
	static long old_hitpoints;
	long hitpoints;

	hitpoints = lara_item->hit_points;

	if (hitpoints < 0)
		hitpoints = 0;
	else if (hitpoints > 1000)
		hitpoints = 1000;

	if (old_hitpoints != hitpoints)
	{
		old_hitpoints = hitpoints;
		health_bar_timer = 40;
	}

	if (health_bar_timer < 0)
		health_bar_timer = 0;

	if (hitpoints <= 250)
	{
		if (BinocularRange)
		{
			if (flash_state)
				S_DrawHealthBar2(hitpoints / 10);
			else
				S_DrawHealthBar2(0);
		}
		else
		{
			if (flash_state)
				S_DrawHealthBar(hitpoints / 10);
			else
				S_DrawHealthBar(0);
		}
	}
	else if (health_bar_timer > 0 || lara.gun_status == LG_READY && lara.gun_type != WEAPON_TORCH || lara.poisoned >= 256)
	{
		if (BinocularRange || SniperOverlay)
			S_DrawHealthBar2(hitpoints / 10);
		else
			S_DrawHealthBar(hitpoints / 10);
	}

	if (PoisonFlag)
		PoisonFlag--;
}

void DrawAirBar(int flash_state)
{
	long air;

	if (lara.air == 1800 || lara_item->hit_points <= 0)
		return;

	air = lara.air;

	if (air < 0)
		air = 0;
	else if (air > 1800)
		air = 1800;

	if (air > 450 || flash_state)
		S_DrawAirBar(100 * air / 1800);
	else
		S_DrawAirBar(0);

	if (lara.Gassed)
	{
		if (lara.dpoisoned < 2048)
			lara.dpoisoned += 2;

		lara.Gassed = 0;
	}
}

void MakeAmmoString(char* string)
{
	char* s;

	s = string;

	if (*string)
	{
		do
		{
			if (*s != 32)
			{
				if (*s - 65 < 0)
					*s -= 47;
				else
					*s -= 53;
			}

		} while (*++s);
	}
}

void InitialisePickUpDisplay()
{
	for (int i = 7; i > -1; i--)
		pickups[i].life = -1;

	PickupY = 128;
	PickupX = 128;
	PickupVel = 0;
	CurrentPickup = 0;
}

void DrawPickups(int timed)
{
	DISPLAYPU* pu;
	long lp;

	pu = &pickups[CurrentPickup];

	if (pu->life > 0)
	{
		if (PickupX > 0)
			PickupX += -PickupX >> 3;
		else
			pu->life--;
	}
	else if (!pu->life)
	{
		if (PickupX < 128)
		{
			if (PickupVel < 16)
				PickupX += ++PickupVel;
		}
		else
		{
			pu->life = -1;
			PickupVel = 0;
		}
	}
	else
	{
		for (lp = 0; lp < 8; lp++)
		{
			if (pickups[(CurrentPickup + lp) % 8].life > 0)
				break;
		}

		if (lp == 8)
		{
			CurrentPickup = 0;
			return;
		}

		CurrentPickup = (CurrentPickup + lp) % 8;
	}

	S_DrawPickup(pu->object_number);
}

void AddDisplayPickup(short object_number)
{
	DISPLAYPU* pu; 

	if (gfCurrentLevel == LVL5_SUBMARINE && object_number == PUZZLE_ITEM1 ||
		gfCurrentLevel == LVL5_OLD_MILL && object_number == PUZZLE_ITEM3)
		object_number = CROWBAR_ITEM;

	for (int i = 0; i < 8; i++)
	{
		pu = &pickups[i];

		if (pu->life < 0)
		{
			pu->life = 45;
			pu->object_number = object_number;
			break;
		}
	}

	DEL_picked_up_object(object_number);
}


void inject_health(bool replace)
{
	INJECT(0x00439C10, FlashIt, replace);
	INJECT(0x00439C50, DrawGameInfo, replace);
	INJECT(0x00439E50, DrawHealthBar, replace);
	INJECT(0x00439FC0, DrawAirBar, replace);
	INJECT(0x0043A0A0, MakeAmmoString, replace);
	INJECT(0x0043A0E0, InitialisePickUpDisplay, replace);
	INJECT(0x0043A130, DrawPickups, replace);
	INJECT(0x0043A240, AddDisplayPickup, replace);
}
