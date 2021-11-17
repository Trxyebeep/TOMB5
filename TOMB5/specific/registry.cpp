#include "../tomb5/pch.h"
#include "registry.h"

bool REG_OpenKey(LPCSTR lpSubKey)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, lpClass, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

void inject_registry(bool replace)
{
	INJECT(0x004BD860, REG_OpenKey, replace);
}
