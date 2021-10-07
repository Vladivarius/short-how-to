/*
Original non-working code posted by 8bitcartridge
here > https://stackoverflow.com/questions/6825555/enumerating-all-subkeys-and-values-in-a-windows-registry-key
Code fixed by Vladivarius
elamon.vlad@gmail.com
27.12.2020.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h> //not sure if this is the only required thing to include
#include <stdio.h>
#include <iostream>

using namespace std;
LSTATUS result;
BYTE databuffer[256];

void EnumerateValues(HKEY hKey, DWORD numValues)
{
	cout << "\nKeys found: " << numValues;
	DWORD dwIndex = 0;
	LPSTR valueName = new CHAR[256];
	DWORD valNameLen;
	DWORD dataType;
	BYTE* data = databuffer;
	DWORD dataSize = 1024;

	for (int i = 0; i < numValues; i++)
	{
		DWORD valNameLen=256;
		dataSize = 256;
		dataType=0;
		result =
			RegEnumValue(hKey,
				dwIndex,
				valueName,
				&valNameLen,
				NULL,
				&dataType,
				data,
				&dataSize);

		if (result != ERROR_SUCCESS) {
			cout << "\nError RegEnumValue > " << result;
			return;
		}
		cout << "\nValue size: " << dataSize << "\nValue data: " << data;
		dwIndex++;
	}
}

void EnumerateSubKeys(HKEY RootKey, const char* subKey, unsigned int tabs = 0)
{
	HKEY hKey;
	DWORD cSubKeys;        //Used to store the number of Subkeys
	DWORD maxSubkeyLen;    //Longest Subkey name length
	DWORD cValues;        //Used to store the number of Subkeys
	DWORD maxValueLen;    //Longest Subkey name length
	DWORD retCode;        //Return values of calls

	result =
		RegOpenKeyEx(RootKey, subKey, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_READ /*KEY_ALL_ACCESS*/, &hKey);
	if (result != ERROR_SUCCESS) {
		cout << "\nError RegOpenKeyEx > " << result;
		return;
	}

	result =
		RegQueryInfoKey(hKey,          // key handle
			NULL,            // buffer for class name
			NULL,            // size of class string
			NULL,            // reserved
			&cSubKeys,        // number of subkeys
			&maxSubkeyLen,    // longest subkey length
			NULL,            // longest class string 
			&cValues,        // number of values for this key 
			&maxValueLen,    // longest value name 
			NULL,            // longest value data 
			NULL,            // security descriptor 
			NULL);            // last write time

	if (result != ERROR_SUCCESS) {
		cout << "\nError RegQueryInfoKey > " << result;
		return;
	}

	if (cSubKeys > 0)
	{
		char currentSubkey[MAX_PATH];

		for (int i = 0; i < cSubKeys; i++) {
			DWORD currentSubLen = MAX_PATH;

			retCode = RegEnumKeyEx(hKey,    // Handle to an open/predefined key
				i,                // Index of the subkey to retrieve.
				currentSubkey,            // buffer to receives the name of the subkey
				&currentSubLen,            // size of that buffer
				NULL,                // Reserved
				NULL,                // buffer for class string 
				NULL,                // size of that buffer
				NULL);                // last write time

			if (retCode == ERROR_SUCCESS)
			{
				for (int i = 0; i < tabs; i++)
					printf("\t");
				printf("(%d) %s\n", i + 1, currentSubkey);

				char* subKeyPath = new char[currentSubLen + strlen(subKey)];
				sprintf(subKeyPath, "%s\\%s", subKey, currentSubkey);
				EnumerateSubKeys(RootKey, subKeyPath, (tabs + 1));
			}
		}
	}
	else
	{
		EnumerateValues(hKey, cValues);
	}

	RegCloseKey(hKey);
}

int main() {
	//Check if a device is plugged in, for example Arduino Uno.
	EnumerateSubKeys(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM");
	getchar();
}