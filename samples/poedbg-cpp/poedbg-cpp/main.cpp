// Part of 'poedbg'. Copyright (c) 2018 maper. Copies must retain this attribution.

__pragma(warning(push, 0))
#include <windows.h>
#include <stdio.h>
__pragma(warning(pop))

// These are the function pointer types for the functions that
// we'll be extracting from poedbg.

typedef int(__stdcall *POEDBG_STANDARD_ROUTINE)();
typedef int(__stdcall *POEDBG_REGISTER_CALLBACK_ROUTINE)(PVOID Callback);

// Here are the callback function pointer types that we'll
// be using.

typedef void(__stdcall *POEDBG_ERROR_CALLBACK)(int Status);
typedef void(__stdcall *POEDBG_PACKET_CALLBACK)(unsigned int Length, BYTE Id, PBYTE Data);

void __stdcall HandleError(int Status)
{
	printf("[ERROR] The 'poedbg' module reported an error code of '%i'.\n", Status);
}

void __stdcall HandlePacketReceive(unsigned int Length, BYTE PacketId, PBYTE PacketData)
{
	printf("[RECEIVED] Packet with ID of '0x%02x' and length of '%u'.\n", PacketId, Length);
}

void __stdcall HandlePacketSend(unsigned int Length, BYTE PacketId, PBYTE PacketData)
{
	printf("[SENT] Packet with ID of '0x%02x' and length of '%u'.\n", PacketId, Length);
}

int main()
{
	printf("Starting 'poedbg' C++ sample...\n");

	// Load and return the handle.
	HMODULE ModuleHandle = LoadLibraryW(L"poedbg.dll");

	if (NULL == ModuleHandle)
	{
		printf("Unable to load the 'poedbg' module.\n");

		getchar();
		return 1;
	}

	// Next we will get the pointers for all of the exports that we might use
	// from poedbg. You only need to get the ones you'll need.

	POEDBG_STANDARD_ROUTINE SdkInitialize = reinterpret_cast<POEDBG_STANDARD_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgInitialize"));
	POEDBG_STANDARD_ROUTINE SdkDestroy = reinterpret_cast<POEDBG_STANDARD_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgDestroy"));
	POEDBG_REGISTER_CALLBACK_ROUTINE SdkRegisterErrorCallback = reinterpret_cast<POEDBG_REGISTER_CALLBACK_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgRegisterErrorCallback"));
	POEDBG_REGISTER_CALLBACK_ROUTINE SdkRegisterPacketSendCallback = reinterpret_cast<POEDBG_REGISTER_CALLBACK_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgRegisterPacketSendCallback"));
	POEDBG_REGISTER_CALLBACK_ROUTINE SdkRegisterPacketReceiveCallback = reinterpret_cast<POEDBG_REGISTER_CALLBACK_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgRegisterPacketReceiveCallback"));
	POEDBG_STANDARD_ROUTINE SdkUnregisterErrorCallback = reinterpret_cast<POEDBG_STANDARD_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgUnregisterErrorCallback"));
	POEDBG_STANDARD_ROUTINE SdkUnregisterPacketSendCallback = reinterpret_cast<POEDBG_STANDARD_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgUnregisterPacketSendCallback"));
	POEDBG_STANDARD_ROUTINE SdkUnregisterPacketReceiveCallback = reinterpret_cast<POEDBG_STANDARD_ROUTINE>(GetProcAddress(ModuleHandle, "PoeDbgUnregisterPacketReceiveCallback"));

	// Now we'll register all of our callbacks. It is important, especially,
	// that we register the error handling callback before we attempt to
	// initialize the poedbg module.

	int Result = SdkRegisterErrorCallback(HandleError);
	if (Result < 0)
	{
		printf("[ERROR] Could not register error callback. Error code: '%i'.\n", Result);

		getchar();
		return 1;
	}

	Result = SdkRegisterPacketReceiveCallback(HandlePacketReceive);
	if (Result < 0)
	{
		printf("[ERROR] Could not register packet send callback. Error code: '%i'.\n", Result);

		getchar();
		return 1;
	}
	Result = SdkRegisterPacketSendCallback(HandlePacketSend);

	if (Result < 0)
	{
		printf("[ERROR] Could not register packet receive callback. Error code: '%i'.\n", Result);

		getchar();
		return 1;
	}

	Result = SdkInitialize();
	if (Result < 0)
	{
		printf("[ERROR] Could not initialize. Error code: '%i'.\n", Result);

		getchar();
		return 1;
	}

	printf("Started successfully.\n");

	getchar();

	Result = SdkDestroy();
	if (Result < 0)
	{
		printf("[ERROR] Could not destroy. Error code: '%i'.\n", Result);

		getchar();
		return 1;
	}

	printf("Removed successfully.\n");

	getchar();
	return 0;
}
