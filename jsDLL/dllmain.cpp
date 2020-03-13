// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

int base = 0x007794f8;

int *get2Point(int base, int p1, int p2)
{
	int iBase, iP1, *iP2;
	if (IsBadReadPtr((LPVOID)base, 4))
		return NULL;
	iBase = *(int *)base;
	if (IsBadReadPtr((LPVOID)(iBase + p1), 4))
		return NULL;
	iP1 = *(int *)(iBase + p1);
	
	iP2 = (int *)(iP1 + p2);
	return iP2;
}

int *get3Point(int base, int p1, int p2, int p3)
{
	int iBase, iP1, iP2, *iP3;

	if (IsBadReadPtr((LPVOID)base, 4))
		return NULL;
	iBase = *(int *)base;

	if (IsBadReadPtr((LPVOID)(iBase + p1), 4))
		return NULL;
	iP1 = *(int *)(iBase + p1);

	if (IsBadReadPtr((LPVOID)(iP1 + p2), 4))
		return NULL;
	iP2 = *(int *)(iP1 + p2);

	iP3 = (int *)(iP2 + p3);
	return iP3;
}

void ModifySun();

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

		for (int i = 0; i < *pCount; i++)
		{
			pFirst[0] = pFirst[1];
			pFirst = (int *)((int)pFirst + 0x50);
		}
		Sleep(100);
	}
}


void ModifyCD()
{
	CreateThread(0, 0, ModifyCDThread, 0, 0, 0);

}

void ModifySun()
{
	int *sun = get2Point(base, 0x868, 0x5578);
	if (sun != NULL)
	{
		if (IsBadWritePtr((LPVOID)sun, 4) == 0)
			*sun = 9000;
	}

}

void ModifyGuanka()
{
	int *pGuanka = get2Point(base, 0x950, 0x4C);
	if (pGuanka != NULL)
		*pGuanka = 36;
}

void ModifyPause()
{
	unsigned char *pCode = (unsigned char *)0x00472B50;
	DWORD dwOldProtect;
	VirtualProtect(pCode, 3, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	pCode[0] = 0xC2;
	pCode[1] = 0x04;
	pCode[2] = 0x00;
	VirtualProtect(pCode, 3, dwOldProtect, NULL);
}

void ModifyMoney()
{
	int *pMoney = get2Point(base, 0x950, 0x50);
	if (pMoney != NULL)
	{
		if (IsBadWritePtr((LPVOID)pMoney, 4) == 0)
			*pMoney = 0;
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		ModifyCD();
		ModifyPause();
		ModifyGuanka();
		ModifyMoney();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

