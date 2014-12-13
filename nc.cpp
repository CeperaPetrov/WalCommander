/*
	Copyright (c) by Valery Goryachev (Wal)
*/
#ifdef _WIN32
#include <Winsock2.h>
#include "resource.h"
#include "w32util.h"
#else
#include <locale.h>
#endif

#include "nc.h"
#include "ncfonts.h"
#include "ncwin.h"
#include "panel.h"

#include <signal.h>
#include "ext-app.h"
#include "wcm-config.h"
#include "color-style.h"
#include "vfs-sftp.h"
#include "charsetdlg.h"
#include "string-util.h"
#include "ltext.h"
#include "globals.h"

#include "intarnal_icons.inc"

cptr<wal::GC> defaultGC;

const char *appName = "Wal Commander";

cfont* (*OldSysGetFont)(Win *w, int id)=0;
cfont* MSysGetFont(Win *w, int id)
{
	if (w) {
		if (w->UiGetClassId() ==  uiClassPanel)
			return panelFont.ptr();

		if (w->UiGetClassId() == uiClassVListWin)
			return  panelFont.ptr();
			
		if (w->UiGetClassId() == uiClassTerminal)
			return  terminalFont.ptr();
			
		if (w->UiGetClassId() == uiClassEditor)
			return  editorFont.ptr();
			
		if (w->UiGetClassId() == uiClassViewer)
			return  viewerFont.ptr();

		if (	w->UiGetClassId() ==  uiClassMenuBar || 
			w->UiGetClassId() ==  uiClassPopupMenu || 
			w->UiGetClassId() ==  uiClassToolTip ||
			w->Parent() && w->UiGetClassId() == uiClassButton)
		{
			return dialogFont.ptr();
		}
	}
	return editorFont.ptr();
}




#ifdef _WIN32
namespace wal {
extern int Win32HIconId;
extern HINSTANCE Win32HInstance;
};
#endif

inline carray<sys_char_t> LTPath(const sys_char_t *fn, const char *ext)
{
	return carray_cat<sys_char_t>(fn, utf8_to_sys(ext).ptr());
}

static  bool InitLocale(const sys_char_t *dir, const char *id)
{
	if (id[0]=='-') return true;
	carray<sys_char_t> fn = 
#ifdef _WIN32
			carray_cat<sys_char_t>(dir, utf8_to_sys("\\ltext.").ptr());
#else
			carray_cat<sys_char_t>(dir, utf8_to_sys("/ltext.").ptr());
#endif

	if (id[0]!='+') 
		return LTextLoad( LTPath(fn.ptr(), id).ptr() );
	
	return LTextLoad( LTPath(fn.ptr(), sys_locale_lang_ter()).ptr()) ||
		LTextLoad( LTPath(fn.ptr(), sys_locale_lang() ).ptr());
};

#ifdef _WIN32
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
#else 
int main(int argc, char **argv)
#endif
{	
	try {
		
		#ifdef _WIN32
			
		#else
			for (int i = 1; i<argc; i++)
			{
				if (argv[i][0]=='-' && argv[i][1]=='-' && !strcmp(argv[i]+2, "dlg"))
					createDialogAsChild = false;
			}
		#endif
	
		#ifdef _WIN32 
		//disable system critical messageboxes (for example: no floppy disk :) )
		SetErrorMode(SEM_FAILCRITICALERRORS);
		#endif

		#ifdef _WIN32
		Win32HInstance = hInstance;
		Win32HIconId = IDI_WCM;
		#endif


#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD( 2, 2 ), &wsaData )) throw_syserr(0,"WSAStartup failed");
#else
		setlocale(LC_ALL, "");
		signal(SIGPIPE, SIG_IGN);
#endif
		
		try {
			InitConfigPath();
			wcmConfig.Load();
			
			const char *langId = wcmConfig.systemLang.ptr() ? wcmConfig.systemLang.ptr() : "+";
#ifdef _WIN32
			InitLocale(carray_cat<sys_char_t>(GetAppPath().ptr(), utf8_to_sys("lang").ptr() ).ptr(), langId);
#else
			if (!InitLocale("install-files/share/wcm/lang", langId))
				InitLocale(UNIX_CONFIG_DIR_PATH "/lang", langId);
#endif

		
		} catch (cexception *ex) {
			fprintf(stderr, "%s\n", ex->message());
			ex->destroy();
		}
		
		InitSSH();

		AppInit();
		
		SetColorStyle(wcmConfig.style.ptr());
		
		OldSysGetFont =SysGetFont;
		SysGetFont=MSysGetFont;

		cfont *defaultFont = SysGetFont(0,0);
		defaultGC = new wal::GC((Win*)0);
		
#ifndef _WIN32
#include "icons/wcm.xpm"
		Win::SetIcon(wcm_xpm);
#endif		

		InitFonts();
		InitOperCharsets();

		SetCmdIcons();

		NCWin ncWin;

		ncWin.Enable();	ncWin.Show();

		InitExtensionApp();		
				
		AppRun();

		dbg_printf("App Quit!!!");

	} catch (cexception *ex)
	{
	#ifdef _WIN32
		MessageBoxA(0, ex->message(), "", MB_OK);
	#else 
		printf("Error: %s\n", ex->message());
	#endif
	}
	return 0;
}
