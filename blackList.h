const LPWSTR BlackListFile = L"blacklist.txt";

WCHAR* LoadBlackList();
bool InBlackList(WCHAR*);