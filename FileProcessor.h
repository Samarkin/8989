typedef void (CALLBACK *PROCESSCALLBACK)(WCHAR* result);
typedef void (CALLBACK *PROGRESSCALLBACK)(int progress);
typedef void (CALLBACK *PROGRESSJOBSIZE)(int size);

typedef struct tagPROCESSFILE
{
	LPWSTR fileName;
	int minLength;
	PROCESSCALLBACK callback;
	PROGRESSCALLBACK progressUpdated;
	PROGRESSJOBSIZE setJobSize;
} PROCESSFILE, *PPROCESSFILE;

DWORD WINAPI ProcessFile(LPVOID arg);