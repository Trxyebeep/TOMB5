#include "../tomb5/pch.h"
#include "registry.h"

bool REG_OpenKey(LPCSTR lpSubKey)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, lpClass, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

bool OpenRegistry(LPCSTR SubKeyName)
{
	char buffer[256];

	if (!SubKeyName)
		return REG_OpenKey("Software\\Core Design\\Tomb Raider Chronicles");

	sprintf(buffer, "%s\\%s", "Software\\Core Design\\Tomb Raider Chronicles", SubKeyName);
	return REG_OpenKey(buffer);
}

void REG_CloseKey()
{
	RegCloseKey(phkResult);
}

void CloseRegistry()
{
	REG_CloseKey();
}

bool REG_KeyWasCreated()
{
	return dwDisposition == REG_CREATED_NEW_KEY;
}

void REG_DeleteValue(char* SubKeyName)
{
	RegDeleteValue(phkResult, SubKeyName);
}

void REG_WriteLong(char* SubKeyName, ulong value)
{
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & value, sizeof(ulong));
}

void REG_WriteBool(char* SubKeyName, bool value)
{
	ulong Lvalue;

	Lvalue = (ulong)value;
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & Lvalue, sizeof(ulong));
}

void REG_WriteString(char* SubKeyName, char* string, long length)
{
	long checkLength;

	if (string)
	{
		if (length < 0)
			checkLength = strlen(string);
		else
			checkLength = length;

		RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*)string, checkLength + 1);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);
}

void REG_WriteDouble(char* SubKeyName, double value)
{
	long length;
	char buf[64];

	length = sprintf(buf, "%.5f", value);
	REG_WriteString(SubKeyName, buf, length);
}

void REG_WriteVoid(char* SubKeyName, void* value, long size)
{
	if (value)
		RegSetValueEx(phkResult, SubKeyName, 0, REG_BINARY, (const BYTE*)value, size);
	else
		RegDeleteValue(phkResult, SubKeyName);
}

void inject_registry(bool replace)
{
	INJECT(0x004BD860, REG_OpenKey, replace);
	INJECT(0x004BDD90, OpenRegistry, replace);
	INJECT(0x004BD8C0, REG_CloseKey, replace);
	INJECT(0x004BDE00, CloseRegistry, replace);
	INJECT(0x004BD8A0, REG_KeyWasCreated, replace);
	INJECT(0x004BDA50, REG_DeleteValue, replace);
	INJECT(0x004BD8E0, REG_WriteLong, replace);
	INJECT(0x004BD910, REG_WriteBool, replace);
	INJECT(0x004BD9E0, REG_WriteString, replace);
	INJECT(0x004BD950, REG_WriteDouble, replace);
	INJECT(0x004BD990, REG_WriteVoid, replace);
}
