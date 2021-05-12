#include "../tomb5/pch.h"
#include "joby.h"
#include "sound.h"
#include "control.h"

void KlaxonTremor()
{
	static short timer;

	if (!(GlobalCounter & 0x1FF))
		SoundEffect(SFX_KLAXON, 0, 0x1000 | SFX_SETVOL);

	if (timer >= 0)
		timer++;

	if (timer > 450)
	{
		if (!(GetRandomControl() & 0x1FF))
		{
			InGameCnt = 0;
			timer = -32 - (GetRandomControl() & 0x1F);
			return;
		}
	}

	if (timer < 0)
	{
		if ((signed int)InGameCnt >= ABS(timer))
		{
			camera.bounce = -(GetRandomControl() % ABS(timer));
			++timer;
		}
		else
			camera.bounce = -(GetRandomControl() % ++InGameCnt);
	}
}


void inject_joby()
{
	INJECT(0x00442C90, KlaxonTremor);
}
