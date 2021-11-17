#pragma once
#include "../global/vars.h"

void inject_registry(bool replace);

bool REG_OpenKey(LPCSTR lpSubKey);
bool OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
bool REG_KeyWasCreated();
void REG_DeleteValue(char* SubKeyName);
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteString(char* SubKeyName, char* string, long length);
void REG_WriteDouble(char* SubKeyName, double value);
void REG_WriteVoid(char* SubKeyName, void* value, long size);
