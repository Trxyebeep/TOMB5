#pragma once
#include "../global/types.h"

bool REG_OpenKey(LPCSTR lpSubKey);
bool OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
bool REG_KeyWasCreated();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyname, bool& value, bool defaultValue);
bool LoadSettings();
void SaveSettings();
bool SaveSetup(HWND hDlg);
