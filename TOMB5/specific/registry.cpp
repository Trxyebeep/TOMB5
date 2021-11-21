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

		RegSetValueEx(phkResult, SubKeyName, 0, REG_SZ, (CONST BYTE*)string, checkLength + 1);
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

bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue)
{
	ulong type;
	ulong cbData;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&value, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
		return 1;

	REG_WriteLong(SubKeyName, defaultValue);	//value not found, write default
	value = defaultValue;
	return 0;
}

bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue)
{
	ulong type;
	ulong cbData;
	ulong data;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&data, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
	{
		value = (bool)data;
		return 1;
	}

	REG_WriteBool(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue)
{
	ulong type;
	ulong cbData;
	long len;

	cbData = length;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, (LPDWORD)&cbData) == ERROR_SUCCESS && type == REG_SZ)
		return 1;

	if (defaultValue)
	{
		REG_WriteBool(SubKeyName, 1);	//this feels wrong.. probably because of the original overloaded names
		len = strlen(defaultValue) + 1;

		if (len > length)
		{
			len = length - 1;
			value[len] = 0;
		}

		memcpy(value, defaultValue, len);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
}

bool REG_ReadDouble(char* SubKeyName, double& value, double defaultValue)
{
	char buf[64];

	if (REG_ReadString(SubKeyName, buf, sizeof(buf), 0))
	{
		value = atof(buf);
		return 1;
	}

	REG_WriteDouble(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
}

bool REG_ReadVoid(char* SubKeyName, void* value, long size, void* defaultValue)
{
	ulong type;
	ulong cbData;

	cbData = size;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, &cbData) == ERROR_SUCCESS && type == REG_BINARY && size == cbData)
		return 1;

	if (defaultValue)
		REG_WriteVoid(SubKeyName, value, size);
	else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
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
	INJECT(0x004BDA80, REG_ReadLong, replace);
	INJECT(0x004BDB00, REG_ReadBool, replace);
	INJECT(0x004BDCC0, REG_ReadString, replace);
	INJECT(0x004BDBA0, REG_ReadDouble, replace);
	INJECT(0x004BDC20, REG_ReadVoid, replace);
}
