#include "encodings.interface.h"

typedef void (CALLBACK *RESULTCALLBACK)(WCHAR* result);
typedef void (CALLBACK *PROGRESSCALLBACK)(int progress);

typedef struct tagPROCESSFILE
{
	LPWSTR fileName;
	int minLength;
	RESULTCALLBACK callback;
	PROGRESSCALLBACK progressUpdated;
	PROGRESSCALLBACK setJobSize;
	FETCHCHAR fetchChar;
	DECODESTRING decodeString;
} PROCESSFILE, *PPROCESSFILE;

DWORD WINAPI ProcessFile(LPVOID arg);
