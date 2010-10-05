#pragma once

#define ALLFILESMASK L"All files(*.*)\0*.*\0"

LPWSTR FileOpenDialog(HWND hWnd, LPCWSTR FMask);
