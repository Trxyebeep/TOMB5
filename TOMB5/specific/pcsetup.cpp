#include "pch.h"
#include "pcsetup.h"
#include "../global/types.h"


void CheckKeyConflicts()
{
	for (int i = 0; i < 18; i++)
	{
		conflict[i] = false;
		for (int j = 0; j < 18; j++)
		{
			if (layout[1][i] == layout[0][i])
			{
				conflict[i] = true;
				break;
			}
		}
	}
}


void inject_pcsetup()
{
	INJECT(0x004ADF40, CheckKeyConflicts);
}
