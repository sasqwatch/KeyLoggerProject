#include <windows.h>
#include <WinUser.h>
#include <tchar.h>
#include <fstream>
#include <locale.h>
#include <winreg.h>
#include <io.h>
#include <direct.h>

#define DEBUG

#define LIST_SIZE 1024
#define BUF_SIZE 1024

#pragma warning(disable:4996)
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

void ScreenCapture(char* path)
{
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPFILEHEADER bFile;                  //비트맵 파일 해더부분을 저장해둘 구조체
	BITMAPINFOHEADER bInfo;          //비트맵 정보 저장을 위한 구조체
	BITMAP bit;
	BITMAPINFO *pih;          //BITMAP 정보를 가지고 있는 구조체
	int PalSize;
	HANDLE hFile;
	DWORD dwWritten, Size;
	HDC hdc = GetDC(NULL);
	HDC S_DC, M_DC;
	HBITMAP hBitmap;
	static int file_count = 1;
	char file_path[255] = { 0, };
	TCHAR File_Path[255] = { 0, };
	

	
	S_DC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);//전체화면 dc얻음
	M_DC = CreateCompatibleDC(S_DC);//전체화면 dc를 호환되는 메모리 dc로
	hBitmap = CreateCompatibleBitmap(S_DC, ScreenWidth, ScreenHeight);//전체화면 dc를 비트맵으로 변환
	SelectObject(M_DC, hBitmap);

	BitBlt(M_DC, 0, 0, ScreenWidth, ScreenHeight, S_DC, 0, 0, SRCCOPY);//메모리 DC에 화면 DC를 복사 받는다.

	DeleteDC(M_DC);
	DeleteDC(S_DC);

	//저장섹션
	//캡쳐한 비트맵에대한 정보를 저장한다.
	GetObject(hBitmap, sizeof(BITMAP), &bit);  
	bInfo.biSize = sizeof(BITMAPINFOHEADER);      
	bInfo.biWidth = bit.bmWidth;
	bInfo.biHeight = bit.bmHeight;
	bInfo.biPlanes = 1;
	bInfo.biBitCount = bit.bmPlanes*bit.bmBitsPixel;
	if (bInfo.biBitCount>8) bInfo.biBitCount = 24;
	bInfo.biCompression = BI_RGB;
	bInfo.biSizeImage = 0;
	bInfo.biXPelsPerMeter = 0;
	bInfo.biYPelsPerMeter = 0;
	bInfo.biClrUsed = 0;
	bInfo.biClrImportant = 0;

	//정보 구조체 + 팔레트 크기만큼 메모리 활당 정보 구조체를 복사
	PalSize = (bInfo.biBitCount == 24 ? 0 : 1 << bInfo.biBitCount) * sizeof(RGBQUAD);
	pih = (BITMAPINFO *)malloc(bInfo.biSize + PalSize);
	pih->bmiHeader = bInfo;

	//비트맵의 크기 구함
	GetDIBits(hdc, hBitmap, 0, bit.bmHeight, NULL, pih, DIB_RGB_COLORS);        //버터에 지정된 형식의 비트맵의 비트를 가져옴.
	bInfo = pih->bmiHeader;

	//비트맵 크기가 구해지지 않았을 때 수작업으로 직접 계산
	if (bInfo.biSizeImage == 0)
	{
		bInfo.biSizeImage = ((((bInfo.biWidth*bInfo.biBitCount) + 31) & ~31) >> 3) * bInfo.biHeight;
	}

	//래스터 데이터를 읽기 위해 메모리 재할당
	Size = bInfo.biSize + PalSize + bInfo.biSizeImage;
	pih = (BITMAPINFO *)realloc(pih, Size);

	//래스터 데이터 읽기.
	GetDIBits(hdc, hBitmap, 0, bit.bmHeight, (PBYTE)pih + bInfo.biSize + PalSize, pih, DIB_RGB_COLORS);

	//비트맵 파일 헤더
	bFile.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + PalSize;
	bFile.bfReserved1 = 0;
	bFile.bfReserved2 = 0;
	bFile.bfSize = Size + sizeof(BITMAPFILEHEADER);
	bFile.bfType = 0x4d42;

	//파일을 생성하고 파일 헤더와 정보 구조체, 팔레트, 래스터 데이터를 출력
	sprintf_s(file_path, 255, "%s\\%d.bmp", path, file_count++);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file_path, strlen(file_path), File_Path,255);

	hFile = CreateFile(File_Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);    //파일 생성
	WriteFile(hFile, &bFile, sizeof(bFile), &dwWritten, NULL);  //파일에 비트캡 헤더 기록
	WriteFile(hFile, pih, Size, &dwWritten, NULL);              //파일에 비트맵 데이터 기록

	memset(file_path, 0, sizeof(file_path));
	ReleaseDC(NULL, hdc);
	CloseHandle(hFile);



}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	char cWindow[255] = { 0, };
	char List[LIST_SIZE] = { 0, };
	char *temp;
	FILE* f;
	static int first = 1;
	int dir_count = 1;
	int ret;
	static char Dir_name[255] = { 0, };

	TCHAR Username[10] = { 0, };
	DWORD UserLen;

	TCHAR tFilePath[255] = { 0, };
	char FilePath[255] = { 0, };

	TCHAR tListPath[255] = { 0, };
	static char ListPath[255] = { 0, };

	char List_Names[1000][50] = { 0, };
	int List_count = 0;

	
	

	


	if (first) {//새로운 디렉터리를 만들기 위해 존재여부 구함

		
		//bmp파일을 저장할 경로를 생성한다.
		GetUserName(Username, &UserLen);
		wsprintf(tFilePath, TEXT("C:\\Users\\%s\\Documents"), Username);
		WideCharToMultiByte(CP_ACP, 0, tFilePath, 255, FilePath, 255, NULL, NULL);

		//list.txt파일경로를 생성한다.
		wsprintf(tListPath, TEXT("C:\\Users\\%s\\Documents\\Controller\\list.txt"), Username);
		WideCharToMultiByte(CP_ACP, 0, tListPath, 255, ListPath, 255, NULL, NULL);

		while (1) {
			sprintf_s(Dir_name, 255, "%s\\bmp%d", FilePath,dir_count);
			ret = _access(Dir_name, 0);
			if (ret != 0) {
				_mkdir(Dir_name);
				break;
			}
			dir_count++;
			memset(Dir_name, 0, 255);
		}
		first = 0;

	}


	if (nCode >= 0)
	{

		if (wParam == WM_LBUTTONDOWN)
		{

			
			//start screensaver
			memset(cWindow, 0, 255);
			GetWindowTextA(GetForegroundWindow(), cWindow, 255);
			
			//list 파일 읽어옴.
			f = fopen(ListPath, "r");
			if (f == NULL) {
#ifdef DEBUG
				MessageBox(NULL, TEXT("fopen error"), TEXT("error"), MB_OK);
#endif
				exit(1);
			}
			fread(List, sizeof(char), LIST_SIZE, f);
			if (strlen(List) <= 0) {
#ifdef DEBUG
				MessageBox(NULL, TEXT("fread error"), TEXT("error"), MB_OK);
#endif
				exit(1);
			}

			temp = strtok(List, ":");
			if (temp == NULL) {
#ifdef DEBUG
				MessageBox(NULL, TEXT("strtok error"), TEXT("error"), MB_OK);
#endif
				exit(1);
			}
			//파일을  : 단위로 나누어 저장한다.
			while (1)
			{
				temp = strtok(NULL, ":");
				if (temp == NULL)
					break;
				else if (strstr(temp, ";") != NULL)
					break;
				else
					strcpy(List_Names[List_count++],temp);

			}
			fclose(f);



			while (1)
			{
				

				if(strstr(cWindow, List_Names[List_count--]) != NULL){
			
					ScreenCapture(Dir_name);

				}
				else if (List_count < 0)
					break;

			}
				



		}

	}


	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


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

//파일이름검색애러.