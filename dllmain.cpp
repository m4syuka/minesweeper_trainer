// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <wingdi.h>
#include <iostream>

typedef __int32(__stdcall* selBox)(int ourX, int ourY);
selBox funcSel;

DWORD WINAPI HackThread(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Func:\n" << "F1 - Extract Tile Layout" << std::endl;
    std::cout << "F2 - Freeze Timer" << std::endl;
    std::cout << "F3 - Show Mines" << std::endl;
    std::cout << "F4 - Inert Mine Tiles" << std::endl;
    std::cout << "F5 - AutoWin" << std::endl;

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"winmine.exe");
    //moduleBase = (uintptr_t)GetModuleHandle(NULL);

    while (1) {
        //show field
        if (GetAsyncKeyState(VK_F1) & 1) {
            uintptr_t*curMines = (uintptr_t *)(moduleBase + 0x5330);
            int* widthField = (int*)(moduleBase + 0x5334);
            int* heightField = (int*)(moduleBase + 0x5338);
            std::cout << "Mines: " << *curMines << std::endl;
            std::cout << "Width: " << *widthField << std::endl;
            std::cout << "Height: " << *heightField << std::endl;

            BYTE* field = (BYTE*)(moduleBase + 0x5340);
            for (int i = 0; i <= *heightField + 1; i++) {
                for (int j = 0; j <= *widthField + 1; j++) {
                    switch (*field)
                    {
                    case 0x10:
                        std::cout << "X" << " ";
                        break;
                    case 0xf:
                        std::cout << "O" << " ";
                        break;
                    case 0x40:
                        std::cout << "-" << " ";
                        break;
                    case 0x8f:
                        std::cout << "*" << " ";
                        break;
                    case 0x41:
                        std::cout << "1" << " ";
                        break;
                    case 0x42:
                        std::cout << "2" << " ";
                        break;
                    case 0x43:
                        std::cout << "3" << " ";
                        break;
                    case 0x44:
                        std::cout << "4" << " ";
                        break;
                    case 0xcc:
                        std::cout << "!" << " ";
                        break;
                    case 0x8a:
                        std::cout << "!" << " ";
                        break;
                    default:
                        std::cout << *field<< " ";
                    }
                    field++;
                }
                std::cout << std::endl;
                field += 0x20 - (*widthField + 2);
            }
        }
        if (GetAsyncKeyState(VK_F2) & 1) {
            DWORD oldprotect;
            VirtualProtect((BYTE*)moduleBase + 0x3830, 6, PAGE_EXECUTE_READWRITE, &oldprotect);
            memset((BYTE*)moduleBase + 0x3830, 0x90, 6);
            VirtualProtect((BYTE*)moduleBase + 0x3830, 6, oldprotect, &oldprotect);

            VirtualProtect((BYTE*)moduleBase + 0x2FF5, 6, PAGE_EXECUTE_READWRITE, &oldprotect);
            memset((BYTE*)moduleBase + 0x2FF5, 0x90, 6);
            VirtualProtect((BYTE*)moduleBase + 0x2FF5, 6, oldprotect, &oldprotect);

            std::cout << "Timer Freeze"<<std::endl;
        }
        if (GetAsyncKeyState(VK_F3) & 1) {
            HWND hwnd = FindWindow(0, L"Minesweeper");
            if (hwnd == 0) {
                std::cout << "Error, couldn't find window. Restart program." << std::endl;
            }
            int* widthField = (int*)(moduleBase + 0x5334);
            int* heightField = (int*)(moduleBase + 0x5338);
            HDC* hdcSrc = (HDC*)((moduleBase + 0x5A20) + (0x8a & 0x1f) * 4);
            BYTE* field = (BYTE*)(moduleBase + 0x5361);
            
            int y = 55;
            for (int j = 1; j <= *heightField; j++, y += 16) {
                int x = 12;
                for (int i = 1; i <= *widthField; i++, x += 16) {
                    HDC hdc = GetDC(hwnd);
                    if (*field == 0x8f)
                        BitBlt(hdc, x, y, 16, 16, *hdcSrc, 0, 0, 0xCC0020u);
                    field++;
                    ReleaseDC(hwnd, hdc);
                }
                field += 0x20 - (*widthField);
            }
            
        }
        if (GetAsyncKeyState(VK_F4) & 1) {
            int* widthField = (int*)(moduleBase + 0x5334);
            int* heightField = (int*)(moduleBase + 0x5338);
            BYTE* field = (BYTE*)(moduleBase + 0x5361);

            for (int j = 1; j <= *heightField; j++) {
                for (int i = 1; i <= *widthField; i++) {
                    if (*field == 0x8f)
                        *field = 0x8e;
                    field++;
                }
                field += 0x20 - (*widthField);
            }
        }
        if (GetAsyncKeyState(VK_F5) & 1) {
            funcSel = (selBox)(moduleBase + 0x31D4);
            funcSel(1, 1);
        }
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr)); 
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

