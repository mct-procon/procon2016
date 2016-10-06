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
		ofn.lpstrFilter = TEXT("�e�L�X�g�t�@�C��(*.txt)\0*.txt\0")
			TEXT("���ׂẴt�@�C��(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("���t�@�C���̓ǂݍ���");
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn))
			return &szFile[0];
		else
			return "\0";
	}
	else return "\0";
}
