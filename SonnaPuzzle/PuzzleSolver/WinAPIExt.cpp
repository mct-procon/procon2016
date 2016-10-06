#include "WinAPIExt.h"

tstring WinAPIExt::ShowFileOpenDialog(HWND hwnd)
{
	static OPENFILENAME ofn;
	static TCHAR szPath[MAX_PATH * 10];
	static TCHAR szFile[MAX_PATH * 10];
	
	if (szPath[0] == TEXT('\0')) {
		GetCurrentDirectory(MAX_PATH * 10, szPath);
	}
	if (ofn.lStructSize == 0) {
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrInitialDir = szPath;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH * 10;
		ofn.lpstrFilter = TEXT("テキストファイル(*.txt)\0*.txt\0")
			TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("問題ファイルの読み込み");
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn))
			return &szFile[0];
		else
			return "\0";
	}
	else return "\0";
}
