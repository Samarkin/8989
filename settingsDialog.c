#include "stdafx.h"
#include "settingsDialog.h"
#include "resource.h"
#include "Commctrl.h"

BOOL CALLBACK DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ChangeValue(HINSTANCE hInst, HWND hWnd, int* current)
{
	DialogBoxParam(hInst, (LPCWSTR)IDD_PREFERENCES, hWnd, DialogProc, (LPARAM)current);
}

INT* editingValue;
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG:
			editingValue = (INT*)lParam;
			SendDlgItemMessage(hDlg, IDC_SPIN1, UDM_SETRANGE, 0, MAKELONG(100, 0));
			SendDlgItemMessage(hDlg, IDC_SPIN1, UDM_SETPOS, 0, *editingValue);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
				{
					DWORD res = SendDlgItemMessage(hDlg, IDC_SPIN1, UDM_GETPOS, 0, *editingValue);
					if(HIWORD(res)) return FALSE;
					*editingValue = res;
					EndDialog(hDlg, TRUE);
					return TRUE;
				}
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}
