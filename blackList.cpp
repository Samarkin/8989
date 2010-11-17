#include "stdafx.h"
#include "blackList.h"

WCHAR* LoadBlackList() {
	HANDLE hFile = CreateFile(BlackListFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	return NULL;
}