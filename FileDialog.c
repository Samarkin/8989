#include "stdafx.h"
#include <commdlg.h>

LPWSTR FileOpenDialog(HWND hWnd, LPCWSTR FMask) {
	// preparing
	OPENFILENAME ofn;
	WCHAR* szFile = malloc(sizeof(WCHAR)*MAX_PATH);
	szFile[0] = '\0';
	FastZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = hWnd;
	ofn.lpstrFilter = FMask;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;
	ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	WCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	// showing
	if (GetOpenFileName(&ofn)) {
		SetCurrentDirectory(path);
		return szFile;
	}
	else {
		DWORD res = CommDlgExtendedError();
		return NULL;
	}
}

LPWSTR FileSaveDialog(HWND hWnd, LPCWSTR FMask, LPCWSTR defExt) {
	// preparing
	OPENFILENAME ofn;
	WCHAR* szFile = malloc(sizeof(WCHAR)*MAX_PATH);
	szFile[0] = '\0';
	FastZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lpstrDefExt = defExt;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = hWnd;
	ofn.lpstrFilter = FMask;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;
	ofn.Flags       = 0;

	WCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	// showing
	if (GetSaveFileName(&ofn)) {
		SetCurrentDirectory(path);
		return szFile;
	}
	else {
		DWORD res = CommDlgExtendedError();
		return NULL;
	}
}