#include "stdafx.h"
#include <commdlg.h>

LPWSTR FileOpenDialog(HWND hWnd, LPCWSTR FMask) {
	// preparing
	OPENFILENAME ofn;
	WCHAR* szFile = new WCHAR[MAX_PATH];
	szFile[0] = '\0';
	FastZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = hWnd;
	ofn.lpstrFilter = FMask;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;
	ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// showing
	if (GetOpenFileName(&ofn)) {
		return szFile;
	}
	else {
		DWORD res = CommDlgExtendedError();
		return NULL;
	}
}