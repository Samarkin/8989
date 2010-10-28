#pragma once

#define ALLFILESMASK L"All files (*.*)\0*.*\0"
#define TXTFILESMASK L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0"

LPWSTR FileOpenDialog(HWND hWnd, LPCWSTR FMask);
LPWSTR FileSaveDialog(HWND hWnd, LPCWSTR FMask, LPCWSTR defExt);
