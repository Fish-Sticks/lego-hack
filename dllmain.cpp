#include "pch.h" // Remove this if your not using PCH.h

#include <Windows.h>
#include <iostream>
#include <vector>

// Make sure your project settings have Multi-Byte character set (Google how to do it lol)

#define vanillaValue(x) x - reinterpret_cast<uintptr_t>(GetModuleHandle(NULL))

struct Vec3
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
};


template <class T> // Setup the template
T* findValue(std::vector<DWORD> offsets)
{
	DWORD base = *(DWORD*)(offsets.at(0)); // Dereferencing first value cuz this is the "base address", the real base would actually be the module base but this is the base for the pointer
	T* result = 0;

	for (int i = 1; i < offsets.size(); i++) {
		if (i == offsets.size() - 1) {	// In C++ arrays end -1 from length due to them starting from zero. This is why we do -1
			result = (T*)(base + offsets.at(i)); // Cast the result to whatever class/struct they wanted and return it.
			return result;
		}
		base = base + offsets.at(i);	// Adding offset and setting up pointer
		if (base) {
			base = *(DWORD*)base;	// Make sure it's safe to dereference (Prevent crashing)
		}
		else {
			return new T();	// Return a new random class to return a valid value. We don't wanna go down a tunnel of errors if the current value in the pointer chain doesn't work.
		}
	}
	return new T();	// Same as last comment
}


void main()
{
	FILE* f;
	DWORD temp;

	AllocConsole(); // Allocate console
	VirtualProtect(&FreeConsole, 1, PAGE_READWRITE, &temp); // Remove protections
	*reinterpret_cast<BYTE*>(&FreeConsole) = 0xC3; // RET opcode (Just makes the function instantly return)
	VirtualProtect(&FreeConsole, 1, temp, &temp); // Replace protections

	freopen_s(&f, "CONOUT$", "w", stdout); // Redirect console write handle
	freopen_s(&f, "CONIN$", "r", stdin); // Redirect console read handle

	SetConsoleTitle("Pranksterria's c++ stuff"); // The reason we had to do all that console stuff was cuz of a dumb check lol

	std::cout << "Successfully injected!\n";

	std::cout << "Vector3 struct size: " << sizeof(Vec3) << std::endl; // Debugging

	while (true) {
		Vec3* result = findValue<Vec3>({					// Grab pointer with offsets
			(DWORD)(GetModuleHandle(nullptr)) + 0x023E6478,
			0x60,
			0x100,
			0x64,
			0xA8,
			0x140
		});

		if (!result) result = new Vec3();	// If the return somehow didn't protect us, this will.

		printf("(%f, %f, %f)\n", result->x, result->y, result->z);	// Display our struct. For non struct* values it'd be . to index but here it is -> because they are values of a pointer

		Sleep(100); // If your dense, this just waits for 1 second (It's in miliseconds)
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
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}