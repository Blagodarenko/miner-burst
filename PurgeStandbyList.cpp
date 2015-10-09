// Очистка памяти от пребуферизованной части прочитанных файлов
//При компиляции надо в свойствах линковщика добавить /MANIFESTUAC:NO  /level = "highestAvailable"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_PRIVILEGE_NOT_HELD        ((NTSTATUS)0xC0000061L)

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemMemoryListInformation = 80, // 80, q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege)
} SYSTEM_INFORMATION_CLASS;

// private
typedef enum _SYSTEM_MEMORY_LIST_COMMAND
{
	MemoryCaptureAccessedBits,
	MemoryCaptureAndResetAccessedBits,
	MemoryEmptyWorkingSets,
	MemoryFlushModifiedList,
	MemoryPurgeStandbyList,
	MemoryPurgeLowPriorityStandbyList,
	MemoryCommandMax
} SYSTEM_MEMORY_LIST_COMMAND;

BOOL GetPrivilege(HANDLE TokenHandle, LPCSTR lpName, int flags)
{
	BOOL bResult;
	DWORD dwBufferLength;
	LUID luid;
	TOKEN_PRIVILEGES tpPreviousState;
	TOKEN_PRIVILEGES tpNewState;

	dwBufferLength = 16;
	bResult = LookupPrivilegeValueA(0, lpName, &luid);
	if (bResult)
	{
		tpNewState.PrivilegeCount = 1;
		tpNewState.Privileges[0].Luid = luid;
		tpNewState.Privileges[0].Attributes = 0;
		//bResult = AdjustTokenPrivileges(TokenHandle, 0, &tpNewState, (LPBYTE)&(tpNewState.Privileges[1]) - (LPBYTE)&tpNewState, &tpPreviousState, &dwBufferLength);
		bResult = AdjustTokenPrivileges(TokenHandle, 0, &tpNewState, sizeof(TOKEN_PRIVILEGES), &tpPreviousState, &dwBufferLength);
		
		if (bResult)
		{
			tpPreviousState.PrivilegeCount = 1;
			tpPreviousState.Privileges[0].Luid = luid;
			tpPreviousState.Privileges[0].Attributes = flags != 0 ? 2 : 0;
			bResult = AdjustTokenPrivileges(TokenHandle, 0, &tpPreviousState, dwBufferLength, 0, 0);
		}
	}
	return bResult;
}

NTSTATUS ClearMem(void)
{

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hToken;
	if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		fprintf(stderr, "\nCan't open current process token\n");
		return 0;
	}

	if (!GetPrivilege(hToken, "SeProfileSingleProcessPrivilege", 1))
	{
		fprintf(stderr, "\nCan't get SeProfileSingleProcessPrivilege\n");
		return 0;
	}

	CloseHandle(hToken);

	HMODULE ntdll = LoadLibrary(L"ntdll.dll");
	if (!ntdll)
	{
		fprintf(stderr, "\nCan't load ntdll.dll, wrong Windows version?\n");
		return 0;
	}

	typedef DWORD NTSTATUS; // ?
	NTSTATUS(WINAPI *NtSetSystemInformation)(INT, PVOID, ULONG) = (NTSTATUS(WINAPI *)(INT, PVOID, ULONG))GetProcAddress(ntdll, "NtSetSystemInformation");
	NTSTATUS(WINAPI *NtQuerySystemInformation)(INT, PVOID, ULONG, PULONG) = (NTSTATUS(WINAPI *)(INT, PVOID, ULONG, PULONG))GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (!NtSetSystemInformation || !NtQuerySystemInformation)
	{
		printf_s("\nCan't get function addresses, wrong Windows version?\n");
		return 0;
	}

	SYSTEM_MEMORY_LIST_COMMAND command = MemoryPurgeStandbyList;
	NTSTATUS status = NtSetSystemInformation(
		SystemMemoryListInformation,
		&command,
		sizeof(SYSTEM_MEMORY_LIST_COMMAND)
		);
	if (status == STATUS_PRIVILEGE_NOT_HELD)
	{
		printf_s("\nInsufficient privileges to execute the memory list command\n");
	}
	else if (status > 0)
	{
		printf_s("\nUnable to execute the memory list command %x\n", status);
	}
	return status;

}


/*
int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hToken;
	if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		fprintf(stderr, "Can't open current process token\n");
		return 1;
	}

	if (!GetPrivilege(hToken, "SeProfileSingleProcessPrivilege", 1))
	{
		fprintf(stderr, "Can't get SeProfileSingleProcessPrivilege\n");
		return 1;
	}

	CloseHandle(hToken);

	HMODULE ntdll = LoadLibrary(L"ntdll.dll");
	if (!ntdll)
	{
		fprintf(stderr, "Can't load ntdll.dll, wrong Windows version?\n");
		return 1;
	}

	typedef DWORD NTSTATUS; // ?
	NTSTATUS (WINAPI *NtSetSystemInformation)(INT, PVOID, ULONG) = (NTSTATUS (WINAPI *)(INT, PVOID, ULONG))GetProcAddress(ntdll, "NtSetSystemInformation");
	NTSTATUS (WINAPI *NtQuerySystemInformation)(INT, PVOID, ULONG, PULONG) = (NTSTATUS (WINAPI *)(INT, PVOID, ULONG, PULONG))GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (!NtSetSystemInformation || !NtQuerySystemInformation)
	{
		fprintf(stderr, "Can't get function addresses, wrong Windows version?\n");
		return 1;
	}

	SYSTEM_MEMORY_LIST_COMMAND command = MemoryPurgeStandbyList;
	NTSTATUS status = NtSetSystemInformation(
		SystemMemoryListInformation,
		&command,
		sizeof(SYSTEM_MEMORY_LIST_COMMAND)
		);
	if (status == STATUS_PRIVILEGE_NOT_HELD)
	{
		fprintf(stderr, "Insufficient privileges to execute the memory list command");
	}
	else if (!NT_SUCCESS(status))
	{
		fprintf(stderr, "Unable to execute the memory list command %p", status);
	}
	return NT_SUCCESS(status);
}

*/
