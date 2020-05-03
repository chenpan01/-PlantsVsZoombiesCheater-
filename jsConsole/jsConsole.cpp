// jsConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int base = 0x007794f8;
HANDLE hProcess;

int *get2Point(int base, int p1, int p2)
{
	int iBase, iP1, *iP2;

	if (!ReadProcessMemory(hProcess, (LPVOID)base, &iBase, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iBase + p1), &iP1, 4, NULL))
	{
		return NULL;
	}

	iP2 = (int *)(iP1 + p2);
	return iP2;
}

int *get3Point(int base, int p1, int p2, int p3)
{
	int iBase, iP1, iP2, *iP3;

	if (!ReadProcessMemory(hProcess, (LPVOID)base, &iBase, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iBase + p1), &iP1, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iP1 + p2), &iP2, 4, NULL))
	{
		return NULL;
	}
	iP3 = (int *)(iP2 + p3);
	return iP3;
}

void ModifySun()
{
	int *sun = get2Point(base, 0x868, 0x5578);
	int sunValue = 9000;
	WriteProcessMemory(hProcess, sun, &sunValue, 4, NULL);
}

void ModifyGuanka()
{
	int *pGuanka = get2Point(base, 0x950, 0x4C);
	int guankaValue = 36;
	WriteProcessMemory(hProcess, pGuanka, &guankaValue, 4, NULL);
}

void ModifyMoney()
{
	int *pMoney = get2Point(base, 0x950, 0x50);
	int moneyValue = 500;
	WriteProcessMemory(hProcess, pMoney, &moneyValue, 4, NULL);
}

void ModifyPause()
{
	unsigned char *pCode = (unsigned char *)0x00472B50;
	DWORD dwOldProtect;
	VirtualProtectEx(hProcess, pCode, 3, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	unsigned char opCode[] = "\xC2\x04\x00";
	WriteProcessMemory(hProcess, pCode, opCode, 3, NULL);
	VirtualProtectEx(hProcess, pCode, 3, dwOldProtect, NULL);
}

DWORD WINAPI ModifyCDThread(
	LPVOID lpParameter   // thread data
	)
{
	while (1)
	{
		//修改阳光
		ModifySun();
		int *pCount = get3Point(base, 0x868, 0x15c, 0x24);
		if (pCount == NULL)
			continue;

		int *pFirst = get3Point(base, 0x868, 0x15c, 0x4c);
		if (pFirst == NULL)
			continue;

		//
		int iCount = 0;
		ReadProcessMemory(hProcess, pCount, &iCount, 4, NULL);
		for (int i = 0; i < iCount; i++)
		{
			//pFirst[0] = pFirst[1];//读和写

			int iRecoveryTime;
			ReadProcessMemory(hProcess, pFirst + 1, &iRecoveryTime, 4, NULL);
			WriteProcessMemory(hProcess, pFirst, &iRecoveryTime, 4, NULL);

			pFirst = (int *)((int)pFirst + 0x50);
		}
		Sleep(100);
	}
}


void ModifyCD()
{
	CreateThread(0, 0, ModifyCDThread, 0, 0, 0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//获取游戏窗口所在进程的进程ID，也就是PID
	HWND hWnd = FindWindow(NULL, TEXT("Plants vs. Zombies"));
	if (NULL == hWnd)
	{
		printf("查找窗口失败\n");
		return 0;
	}
	
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	printf("进程ID:%d\n", dwProcessId);
	
	//获取进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		printf("打开进程失败\n");
		return 0;
	}

	ModifySun();
	ModifyGuanka();
	ModifyMoney();
	ModifyPause();

	ModifyCDThread(0);
	return 0;
}

