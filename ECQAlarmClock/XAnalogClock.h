///////////////////////////////////////////////////////////////////////////////

#ifndef XANALOGCLOCK_H
#define XANALOGCLOCK_H

#include "CDCBuffer.h"

/////////////////////////////////////////////////////////////////////////////
// CXAnalogClock window

class CXAnalogClock : public CStatic
{
// Construction
public:
	CXAnalogClock();
	virtual ~CXAnalogClock();

// Attributes
public:
	CTime GetTime() { return m_time; }
	CSize GetWindowSize();
	BOOL IsClockRunning() { return !m_bStopped; }
	CXAnalogClock& SetBackgroundColor(COLORREF rgb) { m_rgbBackground = rgb; return *this; }
	CXAnalogClock& SetBitmaps(UINT nFaceBitmapId, UINT nDateBitmapId)
	{
		if (m_cdcClockFace.IsValid())
			m_cdcClockFace.DeleteObject();
		if (m_cdcDate.IsValid())
			m_cdcDate.DeleteObject();
		if (m_cdcPrevious.IsValid())
			m_cdcPrevious.DeleteObject();

		m_nFaceBitmapId = nFaceBitmapId;
		m_nDateBitmapId = nDateBitmapId;
		return *this;
	}
	CXAnalogClock& SetAutomaticHandColor(BOOL bAutomaticHandColor)  
	{ m_bAutomaticHandColor = bAutomaticHandColor; return *this; }
	CXAnalogClock& SetHourAdjust(int nHourAdjust)		{ m_nHourAdjust = nHourAdjust; return *this; }
	CXAnalogClock& SetHourHandColor(COLORREF rgb)		{ m_rgbHourHand = rgb; return *this; }
	CXAnalogClock& SetMinuteHandColor(COLORREF rgb)		{ m_rgbMinuteHand = rgb; return *this; }
	CXAnalogClock& SetSecondHandColor(COLORREF rgb)		{ m_rgbSecondHand = rgb; return *this; }
	CXAnalogClock& SetTime(CTime& t)					{ m_time = t; return *this; }
	CXAnalogClock& SetTransparentColor(COLORREF rgb)	{ m_rgbTransparent = rgb; return *this; }
	CXAnalogClock& ShowDate(BOOL bShowDate)				{ m_bShowDate = bShowDate; return *this; }
	CXAnalogClock& ShowSecondHand(BOOL bShowSecondHand)	{ m_bShowSecondHand = bShowSecondHand; return *this; }
	CXAnalogClock& ToggleDebug()						{ m_bDebug = !m_bDebug; return *this; }
	CXAnalogClock& UseSystemTime(BOOL bUseSystemTime)	{ m_bUseSystemTime = bUseSystemTime; return *this; }

// Operations
public:
	void Run();
	void Stop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXAnalogClock)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetDay();
	int GetHour();
	int GetMinute();
	int GetSecond();

	float ConvertToDegrees(float dTime);
	void LoadBitmaps(CDC *pDC);
	void PaintDate(CDC *pDC);
	void PaintMinuteAndHourHands(CDC *pDC, COLORREF crMinuteHand, COLORREF crHourHand, COLORREF crScreenBackground);
	void PaintSecondHand(CDC *pDC, int nSecond, COLORREF rgbSecondHand, COLORREF crScreenBackground);

	CDCBuffer	m_cdcClockFace;
	CDCBuffer	m_cdcDate;
	CDCBuffer	m_cdcPrevious;
	int			m_nPrevMinute;
	BOOL		m_bShowDate;
	BOOL		m_bShowSecondHand;
	BOOL		m_bUseSystemTime;
	BOOL		m_bStopped;
	BOOL		m_bDebug;
	BOOL		m_bAutomaticHandColor;
	CTime		m_time;
	int			m_nHourAdjust;
	COLORREF	m_rgbBackground;
	COLORREF	m_rgbTransparent;
	COLORREF	m_rgbSecondHand;
	COLORREF	m_rgbMinuteHand;
	COLORREF	m_rgbHourHand;
	UINT		m_nFaceBitmapId;
	UINT		m_nDateBitmapId;

	// Generated message map functions
protected:
	//{{AFX_MSG(CXAnalogClock)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XANALOGCLOCK_H
