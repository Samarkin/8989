#pragma once

LPVOID FastCopyMemory(LPVOID addr, SIZE_T len);
VOID FastFillMemory(LPVOID addr, SIZE_T len, BYTE value);
VOID FastZeroMemory(LPVOID addr, SIZE_T len);