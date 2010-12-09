#include "stdafx.h"
#include "blackList.h"
#include <shlwapi.h>

WCHAR* wBlackList = NULL;

WCHAR* LoadBlackList() {
	if(wBlackList)
		delete wBlackList;
	HANDLE hFile = CreateFile(BlackListFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	LARGE_INTEGER sz;
	if(!GetFileSizeEx(hFile, &sz) || sz.HighPart != 0) {
		//ErrorReport(L"processing black list");
		MessageBox(NULL, L"Black list ignored", NULL, MB_OK);
		CloseHandle(hFile);
		return wBlackList = NULL;
	}
	if(sz.LowPart & 1) {
		MessageBox(NULL, L"Black list is not unicode", NULL, MB_OK);
		CloseHandle(hFile);
		return wBlackList = NULL;
	}
	char* bList = new char[sz.LowPart+2];
	DWORD read;
	ReadFile(hFile, bList, sz.LowPart, &read, NULL);
	WCHAR* wList = (LPWSTR)bList;
	wList[sz.LowPart] = '\0';
	CloseHandle(hFile);
	return wBlackList = wList;
}

bool InBlackList(WCHAR* message) {
	return wBlackList && wBlackList[3] != '\0' && StrStr(wBlackList, message) != NULL;
}