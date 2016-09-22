#pragma once
#include <Windows.h>
typedef std::basic_string<TCHAR> tstring;
#if _MSC_VER <= 1600
#include "STL.h"
#endif
namespace WinAPIExt
{
	tstring ShowFileOpenDialog(HWND hwnd);
};

