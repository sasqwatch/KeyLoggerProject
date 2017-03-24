#include <windows.h>
#include <WinUser.h>
#include <tchar.h>
#include <fstream>
#include <locale.h>


#pragma data_seg(".shared")

HHOOK g_hHook = NULL;
HINSTANCE g_hInst = NULL;


#pragma data_seg()
#pragma comment(linker, "/section:.shared,RWS")


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = hinstDLL;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam){	if (nCode >= 0)	{		if (wParam == WM_LBUTTONDOWN)		{			//start screensaver		}	}	return CallNextHookEx(NULL, nCode, wParam, lParam);}
#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) BOOL StartHook()
	{
		BOOL ret = FALSE;
		if (!g_hHook)
		{
			g_hHook = SetWindowsHookEx(WH_MOUSE, MouseProc, g_hInst, 0);
			if (g_hHook)
				ret = TRUE;

		}
		return ret;
	}

	__declspec(dllexport) BOOL StopHook()
	{
		BOOL ret = FALSE;
		if (g_hHook)
		{
			ret = UnhookWindowsHookEx(g_hHook);

			if (ret)
				g_hHook = NULL;
		}
		return ret;
	}

#ifdef __cplusplus
}
#endif
