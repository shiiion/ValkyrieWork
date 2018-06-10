#pragma once

#include <Windows.h>

extern bool Init();
extern void ElevatePPL();
extern void UnlinkHWND(HWND hwnd);
extern void CleanUp();