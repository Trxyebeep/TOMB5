#include "../tomb5/pch.h"
#include "romangod.h"
#include "box.h"
#include "objects.h"
#include "control.h"
#include "../specific/function_stubs.h"
#include "people.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "tomb4fx.h"
#include "debris.h"
#include "sound.h"
#include "effects.h"
#include "effect2.h"
#include "items.h"
#include "gameflow.h"
#include "lara.h"

static RG_TARGET rgt;
static BITE_INFO romangod_hit = { 0, 0, 0, 15 };

void TriggerRomanGodSparks(long x, long y, long z, long xv, long yv, long zv, long god)
{
	SPARKS* sptr;
	
	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 64;

	if (!god)
	{
		sptr->dB = (GetRandomControl() & 0x3F) - 64;
		sptr->dG = sptr->dB >> 1;
	}
	else
	{
		sptr->dG = (GetRandomControl() & 0x3F) - 64;
		sptr->dB = sptr->dG >> 1;
	}

	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 4;
	sptr->Life = 16;
	sptr->sLife = 16;
	sptr->TransType = 2;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;
	sptr->Friction = 34;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = SF_NONE;
}

void TriggerRomanGodMissileFlame(PHD_VECTOR* pos, long fxnum)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = (GetRandomControl() & 0x3F) - 96;
	sptr->sB = sptr->sG >> 1;
	sptr->dR = 0;
	sptr->dG = (GetRandomControl() & 0x3F) - 96;
	sptr->dB = sptr->dG >> 1;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = (GetRandomControl() & 0xF) - 8;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0x3FF) - 512;
	sptr->Yvel = (GetRandomControl() & 0x3FF) - 512;
	sptr->Zvel = (GetRandomControl() & 0x3FF) - 512;
	sptr->Friction = 68;
	sptr->Flags = SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->FxObj = (uchar)fxnum;
	sptr->Scalar = 2;
	sptr->sSize = (GetRandomControl() & 0xF) + 96;
	sptr->Size = sptr->sSize;
	sptr->dSize = sptr->sSize >> 2;
}

void TriggerRomanGodMissile(PHD_3DPOS* pos, short room_number, short num)
{
	FX_INFO* fx;
	short fx_number;

	fx_number = CreateEffect(room_number);

	if (fx_number != -1)
	{
		fx = &effects[fx_number];
		fx->pos.x_pos = pos->x_pos;
		fx->pos.y_pos = pos->y_pos;
		fx->pos.z_pos = pos->z_pos;
		fx->pos.x_rot = pos->x_rot;
		fx->pos.y_rot = pos->y_rot;
		fx->pos.z_rot = 0;
		fx->room_number = room_number;
		fx->counter = 16 * num + 15;
		fx->flag1 = 1;
		fx->object_number = BUBBLES;
		fx->speed = (GetRandomControl() & 0x1F) + 64;
		fx->frame_number = objects[BUBBLES].mesh_index + 16;
	}
}

void TriggerRomanGodLightningGlow(long x, long y, long z, long rgb)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->dG = uchar(rgb >> 8);
	sptr->sG = uchar(rgb >> 8);
	sptr->ColFadeSpeed = 2;
	sptr->dR = uchar(rgb >> 16);
	sptr->sR = uchar(rgb >> 16);
	sptr->TransType = 2;
	sptr->Life = 16;
	sptr->sLife = 16;
	sptr->dB = (uchar)rgb;
	sptr->sB = (uchar)rgb;
	sptr->On = 1;
	sptr->FadeToBlack = 4;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = SF_DEF | SF_SCALE;
	sptr->Scalar = 3;
	sptr->MaxYvel = 0;
	sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
	sptr->Gravity = 0;
	sptr->sSize = (GetRandomControl() & 3) + (rgb >> 24);
	sptr->Size = sptr->sSize;
	sptr->dSize = sptr->sSize;
}

void TriggerRomanGodPowerupFlames(short item_number, long shade)
{
	SPARKS* sptr;

	sptr = &spark[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sB = (GetRandomControl() & 0x3F) - 96;
	sptr->dR = 0;
	sptr->dB = (GetRandomControl() & 0x3F) - 96;

	if (shade < 16)
	{
		sptr->sB = uchar(sptr->sB * shade >> 4);
		sptr->dB = uchar(sptr->dB * shade >> 4);
	}

	sptr->sG = sptr->sB >> 1;
	sptr->dG = sptr->dB >> 1;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->y = 0;
	sptr->x = (GetRandomControl() & 0x1F) - 16;
	sptr->z = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = 0;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = 6;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	sptr->MaxYvel = 0;
	sptr->Gravity = -8 - (GetRandomControl() & 7);
	sptr->Scalar = 2;
	sptr->dSize = 4;
	sptr->sSize = uchar(((GetRandomControl() & 0x1F) + 64) * shade >> 4);
	sptr->Size = sptr->sSize;
}

void TriggerRomangodGravel(ITEM_INFO* item, PHD_VECTOR* pos, long node)
{
	FX_INFO* fx;
	SMOKE_SPARKS* sptr;
	short fx_number;

	GetJointAbsPosition(item, pos, node);

	if (!(GetRandomControl() & 0x1F))
	{
		fx_number = CreateEffect(item->room_number);

		if (fx_number != -1)
		{
			fx = &effects[fx_number];
			fx->pos.x_pos = pos->x;
			fx->pos.y_pos = pos->y;
			fx->pos.z_pos = pos->z;
			fx->room_number = item->room_number;
			fx->pos.x_rot = 0;
			fx->pos.z_rot = 0;
			fx->pos.y_rot = short(2 * GetRandomControl());
			fx->speed = 1;
			fx->fallspeed = 0;
			fx->object_number = BODY_PART;
			fx->shade = 16912;
			fx->flag2 = 9729;
			fx->frame_number = objects[BUBBLES].mesh_index + 2 * (GetRandomControl() & 7);
			fx->counter = 0;
			fx->flag1 = 0;
		}
	}

	if (!(GetRandomControl() & 0xF))
	{
		sptr = &smoke_spark[GetFreeSmokeSpark()];
		sptr->On = 1;
		sptr->sShade = 0;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 32;
		sptr->dShade = (GetRandomControl() & 0xF) + 64;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 64;
		sptr->sLife = sptr->Life;
		sptr->x = pos->x + (GetRandomControl() & 0x1F) - 16;
		sptr->y = pos->y + (GetRandomControl() & 0x1F) - 16;
		sptr->z = pos->z + (GetRandomControl() & 0x1F) - 16;
		sptr->Yvel = 0;
		sptr->Xvel = (GetRandomControl() & 0x7F) - 64;
		sptr->Zvel = (GetRandomControl() & 0x7F) - 64;
		sptr->Friction = 4;
		sptr->Flags = SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 7) + 8;
		sptr->mirror = 0;
		sptr->sSize = (GetRandomControl() & 7) + 8;
		sptr->Size = sptr->sSize;
		sptr->dSize = 2 * sptr->sSize;
	}
}

void InitialiseRomangod(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[ROMAN_GOD].anim_index + 16;
	item->goal_anim_state = 13;
	item->current_anim_state = 13;
	item->frame_number = anims[item->anim_number].frame_base;
	item->status = ITEM_INACTIVE;
	item->pos.x_pos += 486 * phd_sin(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
	item->pos.z_pos += 486 * phd_cos(item->pos.y_rot + 0x4000) >> W2V_SHIFT;
	memset(&rgt, 0, sizeof(RG_TARGET));
}

void RomangodControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* roman;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	AI_INFO info;
	PHD_VECTOR v, s, d;
	PHD_3DPOS p;
	ulong bits;
	long x, y, z, g;
	short rot[2];
	short angle, torso_y, head_x, head, room_number, frame, dframe;
	char light;

	if (CreatureActive(item_number))
	{
		angle = 0;
		torso_y = 0;
		head_x = 0;
		head = 0;
		item = &items[item_number];
		roman = (CREATURE_INFO*) item->data;
		bits = item->meshswap_meshbits;

		if (item->hit_points < 1 && !(bits & 0x10000))
		{
			ExplodeItemNode(item, 16, 0, 8);
			item->mesh_bits |= 0x10000;
			item->meshswap_meshbits |= 0x10000;
		}
		else if (item->hit_points < 75 && !(bits & 0x100))
		{
			ExplodeItemNode(item, 8, 0, 8);
			item->mesh_bits |= 0x100;
			item->meshswap_meshbits |= 0x100;
		}
		else if (item->hit_points < 150 && !(bits & 0x400))
		{
			ExplodeItemNode(item, 10, 0, 32);
			ExplodeItemNode(item, 11, 0, 32);
			item->mesh_bits |= 0x400;
			item->meshswap_meshbits |= 0x400;
		}
		else if (item->hit_points < 225 && !(bits & 0x10))
		{
			ExplodeItemNode(item, 4, 0, 8);
			item->mesh_bits |= 0x10;
			item->meshswap_meshbits = 0x10;
		}

		if (bits != item->meshswap_meshbits)
		{
			item->goal_anim_state = 6;
			item->current_anim_state = 6;
			item->anim_number = objects[ROMAN_GOD].anim_index + 5;
			item->frame_number = anims[item->anim_number].frame_base;
		}

		if (item->hit_points <= 0)
		{
			item->hit_points = 0;

			if (item->current_anim_state != 11)
			{
				item->anim_number = objects[ROMAN_GOD].anim_index + 14;
				item->current_anim_state = 11;
				item->frame_number = anims[item->anim_number].frame_base;
			}
			else if (item->frame_number > anims[item->anim_number].frame_base + 54 && item->frame_number < anims[item->anim_number].frame_base + 74 && item->touch_bits)
			{
				lara_item->hit_points -= 40;
				lara_item->hit_status = 1;
			}
			else if (item->frame_number == anims[item->anim_number].frame_end)
			{
				room_number = item->item_flags[2] & 0xFF;
				r = &room[room_number];
				x = r->x + 4 * (item->draw_room & 0xFFFFFF00) + 512;
				y = r->minfloor + (item->item_flags[2] & 0xFFFFFF00);
				z = r->z + 1024 * (item->draw_room & 0xFF) + 512;
				GetHeight(GetFloor(x, y, z, &room_number), x, y, z);
				TestTriggers(trigger_index, 1, 0);
			}
		}
		else
		{
			roman->enemy = lara_item;
			CreatureAIInfo(item, &info);
			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
			angle = CreatureTurn(item, roman->maximum_turn);

			if (info.ahead)
			{
				torso_y = info.angle >> 1;
				head = info.angle >> 1;
				head_x = info.x_angle;
			}

			roman->maximum_turn = 0;

			switch (item->current_anim_state)
			{
			case 1:
				roman->flags = 0;

				if (roman->mood != ATTACK_MOOD)
				{
					roman->maximum_turn = 0;
					item->goal_anim_state = 7;
				}
				else
					roman->maximum_turn = 364;

				head = angle;

				if (!(item->ai_bits & GUARD) && (GetRandomControl() & 0x1F || info.distance <= 0x100000 && roman->mood == ATTACK_MOOD))
				{
					if (abs(info.angle) <= 20480)
					{
						if (info.ahead && info.distance < 0x100000)
						{
							if (!(GetRandomControl() & 3) && info.bite)
								item->goal_anim_state = 3;
							else if (GetRandomControl() & 1)
								item->goal_anim_state = 4;
							else
								item->goal_anim_state = 5;
						}
						else if (!item->item_flags[0])
						{
							item->goal_anim_state = 2;
							item->item_flags[0] = 5;
						}
						else if (item->trigger_flags == 1 && Targetable(item, &info) && GetRandomControl() & 1)
							item->goal_anim_state = 12;
						else if (!item->trigger_flags && info.distance < 0x640000 && info.bite)
							item->goal_anim_state = 3;
						else
							item->goal_anim_state = 7;
					}
					else
						item->goal_anim_state = 10;
				}
				else
					head = AIGuard(roman);

				break;

			case 2:
				light = 0;
				s.x = -32;
				s.y = 48;
				s.z = 64;
				GetJointAbsPosition(item, &s, 14);
				d.x = -48;
				d.y = 48;
				d.z = 490;
				GetJointAbsPosition(item, &d, 14);
				v.x = (s.x + d.x) >> 1;
				v.y = (s.y + d.y) >> 1;
				v.z = (s.z + d.z) >> 1;
				frame = item->frame_number - anims[item->anim_number].frame_base;

				if (frame > 68 && frame < 130)
				{
					dframe = frame - 68;

					if (dframe > 58)
						dframe = 4 * (62 - dframe);
					else if (dframe > 16)
						dframe = 16;

					g = ((GetRandomControl() & 0x3F) + 128) * dframe >> 4;

					if (!item->trigger_flags)
						TriggerDynamic(v.x, v.y, v.z, 16, 0, g >> 1, g);
					else
						TriggerDynamic(v.x, v.y, v.z, 16, 0, g, g >> 1);

					for (int i = 0; i < 2; i++)
					{
						x = v.x + (GetRandomControl() & 0x7FF) - 1024;
						y = v.y + (GetRandomControl() & 0x7FF) - 1024;
						z = v.z + (GetRandomControl() & 0x7FF) - 1024;
						TriggerRomanGodSparks(x, y, z, 8 * (v.x - x), 8 * (v.y - y), 8 * (v.z - z), item->trigger_flags);
					}
				}

				if (frame > 90 && frame < 130)
				{
					if (!item->trigger_flags)
					{
						d.x = -40;
						d.y = 64;
						d.z = GetRandomControl() % 360;
					}
					else
					{
						d.x = -48;
						d.y = 48;
						d.z = GetRandomControl() % 480;
					}

					GetJointAbsPosition(item, &d, 14);
					g = (GetRandomControl() & 0x3F) + 128;
					s.x = item->pos.x_pos + (GetRandomControl() & 0xFFF) - 2048;
					s.z = item->pos.z_pos + (GetRandomControl() & 0xFFF) - 2048;
					s.y = item->pos.y_pos - (GetRandomControl() & 0x3FF) - 4096;

					for (int i = 0; i < 8; i++)
					{
						if (!rgt.pulptr[i] || !rgt.pulptr[i]->Life)
						{
							if (GlobalCounter & 3 && !light)
							{
								if (!item->trigger_flags)
								{
									rgt.pulptr[i] = TriggerLightning(&s, &d, (GetRandomControl() & 0x3F) + 16, RGBA(0, g >> 1, g, 24), 15, 48, 5);
									TriggerLightningGlow(d.x, d.y, d.z, RGBA(0, g >> 1, g, 16));
								}
								else
								{
									rgt.pulptr[i] = TriggerLightning(&s, &d, (GetRandomControl() & 0x3F) + 16, RGBA(0, g, g >> 1, 24), 15, 48, 5);
									TriggerLightningGlow(d.x, d.y, d.z, RGBA(0, g, g >> 1, 16));
								}
							}

							light = 1;
						}
						else
						{
							rgt.pulptr[i]->Point[3].x = d.x;
							rgt.pulptr[i]->Point[3].y = d.y;
							rgt.pulptr[i]->Point[3].z = d.z;
							TriggerLightningGlow(d.x, d.y, d.z, !item->trigger_flags ? RGBA(0, g >> 1, g, 16) : RGBA(0, g, g >> 1, 16));
						}
					}
				}

				break;

			case 3:
			case 4:
			case 5:
			case 9:
				roman->maximum_turn = 0;

				if (abs(info.angle) < 364)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 364;
				else
					item->pos.y_rot += 364;

				if (item->frame_number > anims[item->anim_number].frame_base + 10)
				{
					r = &room[item->room_number];
					s.x = 0;
					s.y = 0;
					s.z = 0;
					GetJointAbsPosition(item, &s, 16);
					floor = &r->floor[((s.x - r->x) >> 10) * r->x_size + ((s.z - r->z) >> 10)];

					if (floor->stopper)
					{
						for (int i = 0; i < r->num_meshes; i++)
						{
							if (!((r->mesh[i].z ^ s.z) & 0xFFFFFC00) && !((r->mesh[i].x ^ s.x) & 0xFFFFFC00) && r->mesh[i].static_number >= 50 && r->mesh[i].static_number <= 59)
							{
								ShatterObject(0, &r->mesh[i], -64, lara_item->room_number, 0);
								SoundEffect(ShatterSounds[gfCurrentLevel][r->mesh[i].static_number - 50], (PHD_3DPOS*) &r->mesh[i].x, SFX_DEFAULT);
								r->mesh[i].Flags &= ~0x1;
								floor->stopper = 0;
								GetHeight(floor, s.x, s.y, s.z);
								TestTriggers(trigger_index, 1, 0);
							}
						}
					}

					if (!roman->flags && item->touch_bits & 0xC000)
					{
						lara_item->hit_points -= 200;
						lara_item->hit_status = 1;
						CreatureEffectT(item, &romangod_hit, 20, item->pos.y_rot, DoBloodSplat);
						SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
						roman->flags = 1;
					}

					if (!item->trigger_flags)
					{
						d.x = -40;
						d.y = 64;
						d.z = 360;
						GetJointAbsPosition(item, &d, 14);
						d.y = item->pos.y_pos - 64;

						if (item->frame_number == anims[item->anim_number].frame_base + 34 && item->current_anim_state == 3)
						{
							if (item->item_flags[0])
								item->item_flags[0]--;

							TriggerShockwave(&d, 0xA00010, 96, 0x30004080, 0x10000);
							TriggerRomanGodLightningGlow(d.x, d.y, d.z, 0x80004080);
							d.y -= 64;
							TriggerShockwave(&d, 0x600010, 64, 0x30004080, 0x10000);
						}

						frame = anims[item->anim_number].frame_end - item->frame_number;

						if (frame < 16)
							TriggerRomanGodPowerupFlames(item_number, frame);
						else
						{
							frame = item->frame_number - anims[item->anim_number].frame_base;

							if (frame > 16)
								frame = 16;

							TriggerRomanGodPowerupFlames(item_number, frame);
						}
					}
				}

				break;

			case 7:
				roman->flags = 0;
				head = info.angle;

				if (roman->mood != ATTACK_MOOD)
				{
					roman->maximum_turn = 0;

					if (abs(info.angle) < 364)
						item->pos.y_rot += info.angle;
					else if (info.angle < 0)
						item->pos.y_rot -= 364;
					else
						item->pos.y_rot += 364;
				}
				else
					roman->maximum_turn = 1274;

				if (info.distance < 1048576)
					item->goal_anim_state = 1;
				else if (info.bite && info.distance < 3211264)
					item->goal_anim_state = 9;
				else if (item->trigger_flags == 1 && Targetable(item, &info) && !(GetRandomControl() & 3) || !item->trigger_flags && info.distance < 6553600)
					item->goal_anim_state = 1;
				else
					item->goal_anim_state = 7;

				break;

			case 10:
				roman->flags = 0;
				roman->maximum_turn = 0;

				if (info.angle < 0)
					item->pos.y_rot += 364;
				else
					item->pos.y_rot -= 364;

				if (item->frame_number == anims[item->anim_number].frame_end)
					item->pos.y_rot -= 32768;

				break;

			case 12:
				roman->flags = 0;
				roman->maximum_turn = 0;

				if (rgt.dynval)
				{
					rgt.dynval--;
					g = rgt.dynval * ((GetRandomControl() & 0x3F) + 128) >> 4;
					TriggerDynamic(rgt.dynx, rgt.dyny, rgt.dynz, 16, 0, g, g >> 1);
				}

				frame = item->frame_number - anims[item->anim_number].frame_base;

				if (frame >= 10 && frame <= 49)
				{
					if (frame == 34)
					{
						v.x = -48;
						v.y = 48;
						v.z = 1024;
						GetJointAbsPosition(item, &v, 14);
						p.x_pos = -48;
						p.y_pos = 48;
						p.z_pos = 450;
						GetJointAbsPosition(item, (PHD_VECTOR*) &p, 14);
						phd_GetVectorAngles(v.x - p.x_pos, v.y - p.y_pos, v.z - p.z_pos, rot);
						p.y_rot = rot[0];
						p.x_rot = rot[1];
						room_number = item->room_number;
						GetFloor(p.x_pos, p.y_pos, p.z_pos, &room_number);
						TriggerRomanGodMissile(&p, room_number, 1);
						g = (GetRandomControl() & 0x3F) + 128;
						TriggerRomanGodLightningGlow(p.x_pos, p.y_pos, p.z_pos, RGBA(0, g, g >> 1, 64));
						rgt.dynval = 16;
						rgt.dynx = p.x_pos;
						rgt.dyny = p.y_pos;
						rgt.dynz = p.z_pos;

						if (item->item_flags[0])
							item->item_flags[0]--;
					}

					frame -= 10;

					if (frame < 32)
					{
						s.x = -32;
						s.y = 48;
						s.z = 64;
						GetJointAbsPosition(item, &s, 14);
						d.x = -48;
						d.y = 48;
						d.z = 490;
						GetJointAbsPosition(item, &d, 14);

						for (int i = 0; i < 4; i++)
						{
							g = frame * ((GetRandomControl() & 0x3F) + 128) >> 4;

							if (!i)
								TriggerDynamic(d.x, d.y, d.z, 8, 0, g >> 1, g >> 2);

							if (rgt.pulptr[i] && frame && frame != 24)
							{
								if (frame < 16)
								{
									rgt.pulptr[i]->r = 0;
									rgt.pulptr[i]->g = (uchar) g;
									rgt.pulptr[i]->b = (uchar) (g >> 1);
									rgt.pulptr[i]->Life = 56;
								}

								rgt.pulptr[i]->Point[0].x = s.x;
								rgt.pulptr[i]->Point[0].y = s.y;
								rgt.pulptr[i]->Point[0].z = s.z;
								rgt.pulptr[i]->Point[3].x = d.x;
								rgt.pulptr[i]->Point[3].y = d.y;
								rgt.pulptr[i]->Point[3].z = d.z;
							}
							else if (frame < 16)
								rgt.pulptr[i] = TriggerLightning(&s, &d, (GetRandomControl() & 7) + 8, RGBA(0, g, g >> 1, 24), 12, 64, 4);
							else if (frame == 24)
							{
								g = (GetRandomControl() & 0x3F) + 128;
								TriggerLightning(&s, &d, (GetRandomControl() & 0xF) + 24, RGBA(0, g, g >> 1, 32), 13, 64, 4);
							}
						}
					}
				}

				break;
			}
		}

		CreatureTilt(item, 0);
		CreatureJoint(item, 0, torso_y);
		CreatureJoint(item, 1, head_x);
		CreatureJoint(item, 2, head);

		if (item->meshswap_meshbits & 0x400)
		{
			v.x = (GetRandomControl() & 0x1F) - 16;
			v.y = 86;
			v.z = (GetRandomControl() & 0x1F) - 16;
			TriggerRomangodGravel(item, &v, 10);
		}

		if (item->meshswap_meshbits & 0x10)
		{
			v.x = -40;
			v.y = (GetRandomControl() & 0x7F) + 148;
			v.z = (GetRandomControl() & 0x3F) - 32;
			TriggerRomangodGravel(item, &v, 4);
		}

		if (item->meshswap_meshbits & 0x100)
		{
			v.x = (GetRandomControl() & 0x3F) + 54;
			v.y = -170;
			v.z = (GetRandomControl() & 0x1F) + 27;
			TriggerRomangodGravel(item, &v, 8);
		}

		CreatureAnimation(item_number, angle, 0);
	}
}
