#pragma once
#include "../global/vars.h"

void inject_setup(bool replace);

void InitialiseLara(long restore);
void ObjectObjects();
void TrapObjects();
void BaddyObjects();
void InitialiseObjects();
void GetCarriedItems();
void InitialiseGameFlags();
void ClearFootPrints();
void reset_cutseq_vars();
void GetAIPickups();

#define BuildOutsideTable	( (void(__cdecl*)()) 0x004774D0 )
#define SetupGame	( (void(__cdecl*)()) 0x004778F0 )
