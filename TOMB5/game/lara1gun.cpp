#include "../tomb5/pch.h"
#include "delstuff.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "larafire.h"
#include "gameflow.h"
#include "effects.h"
#include "effect2.h"
#include "control.h"
#include "sound.h"
#include "items.h"
#include "sound.h"
#include "tomb4fx.h"
#include "traps.h"
#include "box.h"
#include "collide.h"
#include "switch.h"
#include "sphere.h"
#include "objects.h"

void draw_shotgun_meshes(int weapon_type)
{
	lara.back_gun = WEAPON_NONE;
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[WeaponObjectMesh(weapon_type)].mesh_index + 2 * LM_RHAND];
}

void undraw_shotgun_meshes(int weapon_type)
{
	lara.back_gun = WeaponObject(weapon_type);
	lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];
}

void ready_shotgun(int weapon_type)
{
	lara.gun_status = LG_READY;
	lara.left_arm.z_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.x_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.target = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	lara.left_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	return;
}

void RifleHandler(int weapon_type)
{
	WEAPON_INFO* winfo;
	
	winfo = &weapons[weapon_type];

	if (lara.gun_type != WEAPON_CROSSBOW || LaserSight)
		LaraGetNewTarget(winfo);
	else
		lara.target = 0;

	if (input & IN_ACTION)
		LaraTargetInfo(winfo);

	AimWeapon(winfo, &lara.left_arm);

	if (lara.left_arm.lock)
	{
		lara.torso_x_rot = lara.left_arm.x_rot;
		lara.torso_y_rot = lara.left_arm.y_rot;

		if (camera.old_type != LOOK_CAMERA && !BinocularRange)
		{
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
		}
	}

	if (weapon_type == WEAPON_REVOLVER)
		AnimatePistols(WEAPON_REVOLVER);
	else
		AnimateShotgun(weapon_type);

	if (lara.right_arm.flash_gun)
	{
		if (weapon_type == WEAPON_SHOTGUN || weapon_type == WEAPON_HK)
		{
			if (gfLevelFlags & GF_LVOP_MIRROR_USED && lara_item->room_number == gfMirrorRoom)
			{
					TriggerDynamic_MIRROR((GetRandomControl() & 0xFF) + (phd_sin(lara_item->pos.y_rot) >> 4) + lara_item->pos.x_pos,
						((GetRandomControl() & 0x7F) - 0x23F) + lara_item->pos.y_pos,
						(GetRandomControl() & 0xFF) + (phd_cos(lara_item->pos.y_rot) >> 4) + lara_item->pos.z_pos,
						12, (GetRandomControl() & 0x3F) + 0xC0, (GetRandomControl() & 0x1F) + 0x80, GetRandomControl() & 0x3F);
			}
			else
			{
				TriggerDynamic((GetRandomControl() & 0xFF) + (phd_sin(lara_item->pos.y_rot) >> 4) + lara_item->pos.x_pos,
					((GetRandomControl() & 0x7F) - 0x23F) + lara_item->pos.y_pos,
					(GetRandomControl() & 0xFF) + (phd_cos(lara_item->pos.y_rot) >> 4) + lara_item->pos.z_pos,
					12, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 63);
			}
		}
		else if(weapon_type == WEAPON_REVOLVER)
		{
			PHD_VECTOR pos;

			pos.x = (GetRandomControl() & 0xFF) - 128;
			pos.y = (GetRandomControl() & 0x7F) - 63;
			pos.z = (GetRandomControl() & 0xFF) - 128;
			GetLaraJointPos(&pos, 11);

			if (gfLevelFlags & GF_LVOP_MIRROR_USED && lara_item->room_number == gfMirrorRoom)
				TriggerDynamic_MIRROR(pos.x, pos.y, pos.z, 12, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
			else
				TriggerDynamic(pos.x, pos.y, pos.z, 12, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
		}
	}

	return;
}

void FireShotgun()
{
	struct PHD_VECTOR pos;
	short angles[2];
	short dangles[2];
	int i;
	int r;
	int fired;
	int x;
	int y;
	int z;
	int lp;
	int scatter;

	angles[1] = lara.left_arm.x_rot;
	angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;

	if (!lara.left_arm.lock)
	{
		angles[0] += lara.torso_y_rot;
		angles[1] += lara.torso_x_rot;
	}

	fired = 0;

	if (lara.shotgun_type_carried & 8)
		scatter = 1820;
	else
		scatter = 5460;

	for (i = 0; i < 6; i++)
	{
		r = (GetRandomControl() - 16384) * scatter;

		if (r < 0)
			r += 65535;

		dangles[0] = angles[0] + (r >> 16);
		r = (GetRandomControl() - 16384) * scatter;

		if (r < 0)
			r += 65535;

		dangles[1] = angles[1] + (r >> 16);

		if (FireWeapon(WEAPON_SHOTGUN, lara.target, lara_item, &dangles[0]))
			fired = 1;
	}

	if (fired)
	{
		pos.x = 0;
		pos.y = 228;
		pos.z = 32;
		GetLaraJointPos(&pos, 11);
		x = pos.x;
		y = pos.y;
		z = pos.z;
		pos.x = 0;
		pos.y = 1508;
		pos.z = 32;
		GetLaraJointPos(&pos, 11);
		SmokeCountL = 32;
		SmokeWeapon = WEAPON_SHOTGUN;

		if (lara_item->mesh_bits != 0)
			for (lp = 0; lp < 7; lp++)
				TriggerGunSmoke(x, y, z, pos.x - x, pos.y - y, pos.z - z, 1, SmokeWeapon, 32);

		lara.right_arm.flash_gun = weapons[WEAPON_SHOTGUN].flash_time;
		SoundEffect(SFX_EXPLOSION1, &lara_item->pos, (0x14000 << 8) | SFX_SETPITCH);
		SoundEffect(weapons[WEAPON_SHOTGUN].sample_num, &lara_item->pos, SFX_DEFAULT);
		savegame.Game.AmmoUsed++;
	}
}

void FireHK(int running)
{
	short angles[2];

	if (lara.hk_type_carried & WTYPE_AMMO_1)
		HKTimer = 12;
	else if (lara.hk_type_carried & WTYPE_AMMO_2)
	{
		HKShotsFired++;

		if ((HKShotsFired & 0xFF) == 5)
		{
			HKShotsFired = 0;
			HKTimer = 12;
		}
	}

	angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
	angles[1] = lara.left_arm.x_rot;

	if (!lara.left_arm.lock)
	{
		angles[0] += lara.torso_y_rot;
		angles[1] += lara.torso_x_rot;
	}

	if (running)
	{
		weapons[WEAPON_HK].shot_accuracy = 2184;
		weapons[WEAPON_HK].damage = 1;
	}
	else
	{
		weapons[WEAPON_HK].shot_accuracy = 728;
		weapons[WEAPON_HK].damage = 3;
	}

	if (FireWeapon(WEAPON_HK, lara.target, lara_item, angles))
	{
		SmokeCountL = 12;
		SmokeWeapon = WEAPON_HK;
		lara.right_arm.flash_gun = weapons[WEAPON_HK].flash_time;
	}
}

void FireCrossbow(PHD_3DPOS* Start)
{
	short* ammo;
	ITEM_INFO* item;
	short item_number;

	ammo = get_current_ammo_pointer(WEAPON_CROSSBOW);

	if (*ammo)
	{
		lara.has_fired = 1;
		lara.Fired = 1;
		item_number = CreateItem();

		if (item_number != NO_ITEM)
		{
			item = &items[item_number];
			item->object_number = CROSSBOW_BOLT;
			item->shade = -15856;
			item->room_number = lara_item->room_number;
			item->pos.x_pos = Start->x_pos;
			item->pos.y_pos = Start->y_pos;
			item->pos.z_pos = Start->z_pos;
			InitialiseItem(item_number);
			item->pos.x_rot = Start->x_rot;
			item->pos.y_rot = Start->y_rot;
			item->pos.z_rot = Start->z_rot;
			item->speed = 512;
			AddActiveItem(item_number);

			if (*ammo != -1)
				*ammo -= 1;

			SoundEffect(SFX_LARA_CROSSBOW, 0, SFX_DEFAULT);
			savegame.Game.AmmoUsed++;
		}
	}
}

void ControlCrossbow(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	ITEM_INFO** itemslist;
	MESH_INFO** staticslist;
	//char* cptr;
	short room_number;
	long speed;

	item = &items[item_number];
	speed = item->speed;
	item->pos.x_pos += (speed * phd_cos(item->pos.x_rot) >> 14) * phd_sin(item->pos.y_rot) >> 14;
	item->pos.y_pos += speed * phd_sin(-item->pos.x_rot) >> 14;
	item->pos.z_pos += (speed * phd_cos(item->pos.x_rot) >> 14) * phd_cos(item->pos.y_rot) >> 14;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	itemslist = itemlist;
	staticslist = staticlist;
	GetCollidedObjects(item, 128, 1, itemslist, staticslist, 1);

	if (*itemslist)
	{
		target = *itemslist;

		while (!(gfLevelFlags & GF_LVOP_TRAIN) || target->object_number != GRAPPLING_TARGET)
		{
			++itemslist;
			target = *itemslist;
			if (!target)
				return;
		}

		TriggerGrapplingEffect(target->pos.x_pos, target->pos.y_pos + 32, target->pos.z_pos);
		TestTriggersAtXYZ(target->pos.x_pos, target->pos.y_pos, target->pos.z_pos, target->room_number, 1, 0);
		ExplodeItemNode(item, 0, 0, 256);
		SoundEffect(SFX_SMASH_METAL, &item->pos, 0);
		KillItem(item_number);
		target->mesh_bits <<= 1;
	}
}

void draw_shotgun(int weapon_type)
{
	ITEM_INFO* item;

	if (lara.weapon_item == NO_ITEM)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		item->object_number = WeaponObject(weapon_type);
		item->anim_number = objects[item->object_number].anim_index + 1;
		item->frame_number = anims[item->anim_number].frame_base;
		item->status = ITEM_ACTIVE;
		item->goal_anim_state = 1;
		item->current_anim_state = 1;
		item->room_number = 255;
		lara.left_arm.frame_base = objects[item->object_number].frame_base; 
		lara.right_arm.frame_base = objects[item->object_number].frame_base;
	}
	else
		item = &items[lara.weapon_item];

	AnimateItem(item);

	if (item->current_anim_state != 0 &&
		item->current_anim_state != 6)
	{
		if (item->frame_number - anims[item->anim_number].frame_base == weapons[weapon_type].draw_frame)
			draw_shotgun_meshes(weapon_type);
		else if (lara.water_status == LW_UNDERWATER)
			item->goal_anim_state = 6;
	}
	else
		ready_shotgun(weapon_type);

	lara.left_arm.frame_base = anims[item->anim_number].frame_ptr; 
	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.left_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base; 
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.left_arm.anim_number = item->anim_number; 
	lara.right_arm.anim_number = item->anim_number;
	return;
}

void undraw_shotgun(int weapon_type)
{
	ITEM_INFO* item; 

	item = &items[lara.weapon_item];
	item->goal_anim_state = 3;
	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		lara.gun_status = LG_NO_ARMS;
		lara.target = 0;
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		KillItem(lara.weapon_item);
		lara.weapon_item = NO_ITEM;
		lara.right_arm.frame_number = 0;
		lara.left_arm.frame_number = 0;
	}
	else if (item->current_anim_state == 3)
	{	
			if (anims[item->anim_number].frame_base == item->frame_number - 21)
				undraw_shotgun_meshes(weapon_type);
	}

	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.left_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.left_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.right_arm.anim_number = item->anim_number;
	lara.left_arm.anim_number = item->anim_number;
	return;
}

void AnimateShotgun(int weapon_type)
{
	static int m16_firing;
	int running;
	ITEM_INFO* item;

	if (HKTimer)
	{
		m16_firing = 0;
		HKTimer--;
	}

	if (SmokeCountL)
	{
		PHD_VECTOR pos;

		if (SmokeWeapon == WEAPON_HK)
		{
			pos.x = 0;
			pos.y = 228;
			pos.z = 96;
		}
		else if (SmokeWeapon == WEAPON_SHOTGUN)
		{
			pos.x = -16;
			pos.y = 228;
			pos.z = 32;
		}

		GetLaraJointPos(&pos, 11);

		if (lara_item->mesh_bits)
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	item = &items[lara.weapon_item];

	if (weapon_type == WEAPON_HK && lara_item->speed != 0)
		running = 1;
	else
		running = 0;

	switch (item->current_anim_state)
	{
	case 0:
		m16_firing = 0;
		HKTimer = 0;
		HKShotsFired = 0;

		if (lara.water_status == LW_UNDERWATER || running)
			item->goal_anim_state = 6;
		else if (input & IN_ACTION && lara.target == 0 || lara.left_arm.lock)
			item->goal_anim_state = 2;
		else
			item->goal_anim_state = 4;

		break;

	case 2:
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->goal_anim_state = 4;

			if (lara.water_status != LW_UNDERWATER && !running)
			{
				if ((input & IN_ACTION) && (lara.target == 0 || lara.left_arm.lock))
				{
					if (weapon_type == WEAPON_CROSSBOW)
					{
						FireCrossbow(0);
						item->goal_anim_state = 2;
					}
					else
					{
						if (weapon_type == WEAPON_HK)
						{
							if (!(lara.hk_type_carried & (WTYPE_AMMO_1 | WTYPE_AMMO_2)) || !HKTimer)
							{
								FireHK(0);
								m16_firing = 1;
								item->goal_anim_state = 2;

								if (lara.hk_type_carried & WTYPE_SILENCER)
									SoundEffect(SFX_HK_SILENCED, 0, SFX_DEFAULT);
								else
								{
									SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 83888140);
									SoundEffect(SFX_HK_FIRE, &lara_item->pos, SFX_DEFAULT);
								}
							}
							else
								item->goal_anim_state = 0;
						}
						else
						{
							FireShotgun();
							item->goal_anim_state = 2;
						}
					}
				}
				else if (lara.left_arm.lock)
					item->goal_anim_state = 0;
			}

			if (item->goal_anim_state != 2 && m16_firing && !(lara.hk_type_carried & WTYPE_SILENCER))
			{
				StopSoundEffect(SFX_HK_FIRE);
				SoundEffect(SFX_HK_STOP, &lara_item->pos, SFX_DEFAULT);
				m16_firing = 0;
			}

			if (item->frame_number - anims[item->anim_number].frame_base == 12 && weapon_type == WEAPON_SHOTGUN)
				TriggerGunShell(1, SHOTGUNSHELL, WEAPON_SHOTGUN);
		}
		else if (m16_firing)
		{
			if (lara.hk_type_carried & WTYPE_SILENCER)
				SoundEffect(SFX_HK_SILENCED, 0, SFX_DEFAULT);
			else
			{
				SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 83888140);
				SoundEffect(SFX_HK_FIRE, &lara_item->pos, SFX_DEFAULT);
			}
		}
		else if (weapon_type == WEAPON_SHOTGUN && !(input & IN_ACTION) && !lara.left_arm.lock)
			item->goal_anim_state = 4;

		if (item->frame_number - anims[item->anim_number].frame_base == 12 && weapon_type == WEAPON_SHOTGUN)
			TriggerGunShell(1, SHOTGUNSHELL, WEAPON_SHOTGUN);

		break;

	case 6:
		m16_firing = 0;
		HKTimer = 0;
		HKShotsFired = 0;

		if (lara.water_status == LW_UNDERWATER || running)
		{
			if (input & IN_ACTION && lara.target == 0 || lara.left_arm.lock)
				item->goal_anim_state = 8;
			else
				item->goal_anim_state = 7;
		}
		else
			item->goal_anim_state = 0;

		break;

	case 8:
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->goal_anim_state = 7;
			if (running)
			{
				if (input & IN_ACTION)
				{
					if (lara.target == 0 || lara.left_arm.lock)
					{
						if (!(lara.hk_type_carried & (WTYPE_AMMO_1 | WTYPE_AMMO_2)) || !HKTimer)
						{
							FireHK(1);
							m16_firing = 1;
							item->goal_anim_state = 8;

							if (lara.hk_type_carried & WTYPE_SILENCER)
								SoundEffect(SFX_HK_SILENCED, 0, SFX_DEFAULT);
							else
							{
								SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 83888140);
								SoundEffect(SFX_HK_FIRE, &lara_item->pos, SFX_DEFAULT);
							}
						}
						else
							item->goal_anim_state = 6;
					}
				}
				else if (lara.left_arm.lock)
					item->goal_anim_state = 6;
			}

			if (item->goal_anim_state != 8 && m16_firing && !(lara.hk_type_carried & WTYPE_SILENCER))
			{
				StopSoundEffect(SFX_HK_FIRE);
				SoundEffect(SFX_HK_STOP, &lara_item->pos, SFX_DEFAULT);
				m16_firing = 0;
			}
		}
		else if (m16_firing)
		{
			if (lara.hk_type_carried & WTYPE_SILENCER)
				SoundEffect(SFX_HK_SILENCED, 0, SFX_DEFAULT);
			else
			{
				SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 83888140);
				SoundEffect(SFX_HK_FIRE, &lara_item->pos, SFX_DEFAULT);
			}
		}

		break;

	default:
		break;
	}

	AnimateItem(item);
	lara.left_arm.frame_base = anims[item->anim_number].frame_ptr; 
	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.left_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base; 
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.left_arm.anim_number = item->anim_number; 
	lara.right_arm.anim_number = item->anim_number;
	return;
}

void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item)
{
	if (!(baddie->flags & 0x8000))
	{
		if (baddie == lara_item && lara_item->hit_points > 0)
		{
			lara_item->hit_points -= 50;

			if (!(room[item->room_number].flags & RF_FILL_WATER) && baddie->hit_points < 51)
				LaraBurn();
		}
		else
		{
			if (item->item_flags[2] == 0)
			{
				baddie->hit_status = 1;

				if (!objects[baddie->object_number].undead)
				{
					HitTarget(baddie, NULL, 30, 1);
					if (baddie != lara_item)
					{
						savegame.Game.AmmoHits++;

						if (baddie->hit_points <= 0)
						{
							savegame.Level.Kills++;
							CreatureDie((baddie - items) / sizeof(ITEM_INFO), 1);
						}
					}
				}
			}
		}
	}
}

void TriggerGrapplingEffect(long x, long y, long z)
{
	long size;
	long lp;
	SMOKE_SPARKS* sptr;

	for (lp = 0; lp < 24; lp++)
	{
		sptr = &smoke_spark[GetFreeSmokeSpark()];
		sptr->On = 1;
		sptr->sShade = (GetRandomControl() & 0xF) + 40;
		sptr->dShade = (GetRandomControl() & 0xF) + 64;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 16;
		sptr->Life = sptr->sLife = (GetRandomControl() & 3) + 40;
		sptr->TransType = 2;
		sptr->x = x + (GetRandomControl() & 0x1F) - 16;
		sptr->y = y + (GetRandomControl() & 0x1F) - 16;
		sptr->z = z + (GetRandomControl() & 0x1F) - 16;
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) << 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) << 1;

		if (lp < 12)
		{
			sptr->Yvel = (GetRandomControl() & 0x1F);
			sptr->Friction = 64;
		}
		else
		{
			sptr->Yvel = (GetRandomControl() & 0x1FF) + 256;
			sptr->Friction = 82;
		}

		sptr->Flags = 16;
		sptr->RotAng = (GetRandomControl() & 0xFFF);
		sptr->RotAdd = (GetRandomControl() & 0x40) - 32;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 0xF) + 48;
		sptr->dSize = (unsigned char)(size >> 1);
		sptr->sSize = (unsigned char)(size >> 2);
		sptr->Size = (unsigned char)(size >> 2);
		sptr->mirror = 0;
	}
}

void inject_lara1gun()
{
	INJECT(0x0044DBB0, draw_shotgun_meshes);
	INJECT(0x0044DBF0, undraw_shotgun_meshes);
	INJECT(0x0044DC30, ready_shotgun);
	INJECT(0x0044DCC0, RifleHandler);
	INJECT(0x0044E110, FireShotgun);
	INJECT(0x0044E380, FireHK);
	INJECT(0x0044E4B0, FireCrossbow);
//	INJECT(0x0044E5E0, CrossbowHitSwitchType78);
	INJECT(0x0044E8B0, ControlCrossbow);
	INJECT(0x0044EAC0, draw_shotgun);
	INJECT(0x0044ECA0, undraw_shotgun)
	INJECT(0x0044EE00, AnimateShotgun);
	INJECT(0x0044F690, DoGrenadeDamageOnBaddie);
	INJECT(0x0044F7C0, TriggerGrapplingEffect);
}
