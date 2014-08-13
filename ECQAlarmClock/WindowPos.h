#ifndef WINDOWPOS_H
#define WINDOWPOS_H


BOOL RestoreWindowPos(HWND hWnd, 
					  LPCTSTR lpszEntry, 
					  WINDOWPLACEMENT *pWP = NULL,
					  LPCTSTR lpszDefaultPos = NULL);

void SaveWindowPos(HWND hWnd, LPCTSTR lpszEntry);


#endif //WINDOWPOS_H
