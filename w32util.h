#if !defined( W32UTIL_H ) && defined(_WIN32)
#define W32UTIL_H

#include <windows.h>
#include <wal.h>

wal::carray<wchar_t> GetAppPath();

class RegKey {
	HKEY key;
public:
	RegKey():key(0){}
	bool Ok(){ return key != 0; }
	void Close();
	HKEY Key() { return key; }
	bool Open(HKEY root, const wchar_t *name, REGSAM sec = KEY_READ);
	bool Create(HKEY root, const wchar_t *name, REGSAM sec = KEY_WRITE|KEY_READ);
	wal::carray<wchar_t> GetString(const wchar_t *name=0, const wchar_t *def = 0);
	wal::carray<wchar_t> SubKey(int n);
	~RegKey();
};

class WinDLL {
	const wchar_t *name;
	bool mustfree;
	HMODULE handle;
public:
	WinDLL(const wchar_t *_name);
	FARPROC GetProcAddress(const char * procName){ return handle ? ::GetProcAddress(handle, procName) : 0; }
	~WinDLL();
};

extern WinDLL winShell32;

#endif
