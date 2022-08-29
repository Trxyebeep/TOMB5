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
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyname, bool& value, bool defaultValue);
bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue);
bool REG_ReadDouble(char* SubKeyName, double& value, double defaultValue);
bool REG_ReadVoid(char* SubKeyName, void* value, long size, void* defaultValue);

#define SaveSettings	( (void(__cdecl*)()) 0x004BE7E0 )
#define LoadSettings	( (bool(__cdecl*)()) 0x004BDE20 )
