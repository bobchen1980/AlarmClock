#ifndef XTOOLTIP_H
#define XTOOLTIP_H


//=============================================================================
class CXToolTip
//=============================================================================
{
// Construction
public:
	CXToolTip();
	virtual ~CXToolTip();

// Attributes
public:
	HWND m_hWnd;
	TCHAR *m_pszText;
	void Hide();
	void Show(LPCTSTR lpszText, int x, int y, BOOL bConfine = FALSE);

// Overrides
public:
	virtual BOOL Create(HWND hParent, HINSTANCE hInstance);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	SIZE GetCursorOffset(HWND hWnd);

	TCHAR		m_szClassName[50];
	HFONT		m_pFont;
	HINSTANCE	m_hInstance;		// app instance
	HWND		m_hParent;			// parent hwnd
	int			m_X, m_Y;			// x, y of current tip
	CRect		m_rect;
	CRect		m_rectParent;
	SIZE		m_sizeOffset;
};

#endif //XTOOLTIP_H
