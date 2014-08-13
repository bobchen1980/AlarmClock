// TitleMenu.h: interface for the CTitleMenu class.
// Based on the code of Per Fikse(1999/06/16) on codeguru.earthweb.com
// Author: Arthur Westerman
// Bug reports by : Brian Pearson 
//////////////////////////////////////////////////////////////////////

#ifndef TITLEMENU_H
#define TITLEMENU_H

class CTitleMenu : public CMenu
{
	typedef UINT (CALLBACK* LPFNDLLFUNC1)(HDC, CONST PTRIVERTEX, DWORD, 
		CONST PVOID, DWORD, DWORD);

// Construction
public:
	CTitleMenu();
	virtual ~CTitleMenu();

// Attributes
protected:
	CFont m_Font;
	CString m_strTitle;

	LPFNDLLFUNC1 dllfunc_GradientFill;
	HINSTANCE hinst_msimg32;
	long clRight;
	long clLeft;
	long clText;
	BOOL bDrawEdge;
	BOOL m_bCanDoGradientFill;
	UINT flag_edge;

// Operations
public:
	void AddMenuTitle(LPCTSTR lpszTitle);

protected:
	void CreatePopupMenuTitleFont();
	BOOL GradientFill(HDC hdc,
					  CONST PTRIVERTEX pVertex,
					  DWORD dwNumVertex,
					  CONST PVOID pMesh,
					  DWORD dwNumMesh,
					  DWORD dwMode);
	
// Implementation
public:
	static COLORREF LightenColor(COLORREF col,double factor);
	void SetColor(long cl) {clLeft=cl;};
	void SetGradientColor(long cl) {clRight=cl;};
	void SetTextColor(long cl) {clText=cl;};
	// See ::DrawEdge for flag values
	void SetEdge(BOOL shown, UINT remove = 0, UINT add = 0) 
	{
		bDrawEdge = shown; 
		(flag_edge ^= remove) |= add;
	};

	long GetColor() { return clLeft; };
	long GetGradientColor() { return clRight; };
	long GetTextColor() { return clText; };
	long GetEdge() { return flag_edge; };

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //TITLEMENU_H
