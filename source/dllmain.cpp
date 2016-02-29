// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "memory.h"
#include "engine.h"

Engine* hacking_engine = new Engine();
uint32_t original_flipscreen;

__declspec(naked) void HookedFlipScreen()
{
	_asm
	{
		pushad
		pushfd
	}

	// run our hacking routine each time the game flips the screen buffers
	// might optimize this to not drop the framerate on "slow" machines
	hacking_engine->Run();
	
	_asm
	{
		popfd
		popad
		jmp original_flipscreen
	}
}

// prevent target from crashing, by not doing this inside the hooked function
uint32_t original_dispatch_message = reinterpret_cast<uint32_t>(&DispatchMessageA) + 5;
_declspec(naked) void HookedDispatchMessageA()
{
	static bool initialized = false;
	
	_asm
	{
		pushad
		pushfd
	}

	// check if game is ready and hack not yet initialized
	if (!initialized && GetModuleHandleA("CShell.dll") && GetModuleHandleA("ClientFx.fxd"))
	{
		// Initialize not so awesome hacking engine
		hacking_engine->Initialize(GetModuleHandleA("CShell.dll"));

		// Backup the memory we're going to modify so we can restore it if needed
		// Also required to detour the game's checks for modified values
		hacking_engine->Backup();

		// Bypass the shitty 28_3 Client Error
		hacking_engine->DetourWeaponCheck();

		// Initialize fancy game hook
		hacking_engine->HookFlipScreen(reinterpret_cast<uint32_t>(&HookedFlipScreen), &original_flipscreen);
		initialized = true;
	}

	_asm
	{
		popfd
		popad
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp original_dispatch_message
	}
}

bool HookDispatchMessageA(uint32_t routine)
{
	uint32_t address_of_function = reinterpret_cast<uint32_t>(&DispatchMessageA);
	uint8_t shell_code[] = "\xE9\x00\x00\x00\x00";
	*(uint32_t*)(shell_code + 1) = routine - address_of_function - 5;

	return PlaceBytes(reinterpret_cast<void*>(address_of_function), shell_code, 5);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: 
		if (!HookDispatchMessageA(reinterpret_cast<uint32_t>(HookedDispatchMessageA)))
		{
			MessageBoxA(NULL, "Something went incredibly wrong", "Uh oh..", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		break; 

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}