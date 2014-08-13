#include "stdafx.h"
#include "XAnalogClock.h"
#include "math.h"
#include "WuLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)			// disable bogus deprecation warning

#pragma message("automatic link to msimg32.lib")
#pragma comment(lib, "msimg32.lib")		// needed for TransparentBlt()

///////////////////////////////////////////////////////////////////////////////
// some definitions
#define DATE_BOX_WIDTH		18
#define DATE_BOX_HEIGHT		14
#define DATE_BOX_XPOS		67
#define DATE_BOX_YPOS		43
#define SECOND_HAND_LENGTH	35.0
#define MINUTE_HAND_LENGTH	39.0
#define HOUR_HAND_LENGTH	28.0
#define SECOND_HAND_COLOR	RGB(254,80,90);//RGB(220,20,60)
#define MINUTE_HAND_COLOR	RGB(107,210,255);//RGB(0,0,0)
#define HOUR_HAND_COLOR		RGB(107,210,255);//RGB(0,0,0)
#define PI					3.1415926535

///////////////////////////////////////////////////////////////////////////////
// CXAnalogClock

BEGIN_MESSAGE_MAP(CXAnalogClock, CStatic)
	//{{AFX_MSG_MAP(CXAnalogClock)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXAnalogClock::CXAnalogClock()
{
	m_nPrevMinute         = -1;
	m_bShowDate           = TRUE;
	m_bShowSecondHand     = TRUE;
	m_bUseSystemTime      = TRUE;
	m_bStopped            = TRUE;
	m_bDebug              = FALSE;
	m_bAutomaticHandColor = FALSE;
	m_time                = CTime::GetCurrentTime();
	m_nHourAdjust         = 0;
	m_rgbBackground       = RGB(255,0,255);
	m_rgbTransparent      = RGB(255,0,255);
	m_rgbSecondHand       = SECOND_HAND_COLOR;
	m_rgbMinuteHand       = MINUTE_HAND_COLOR;
	m_rgbHourHand         = HOUR_HAND_COLOR;
	m_nFaceBitmapId       = 0;
	m_nDateBitmapId       = 0;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXAnalogClock::~CXAnalogClock()
{
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXAnalogClock::PreSubclassWindow()
{
	CClientDC dc(NULL);
	LoadBitmaps(&dc);
	CStatic::PreSubclassWindow();
}

///////////////////////////////////////////////////////////////////////////////
// GetWindowSize
CSize CXAnalogClock::GetWindowSize()
{
	CClientDC dc(NULL);
	LoadBitmaps(&dc);
	return m_cdcClockFace.m_sizeBitmap;
}

///////////////////////////////////////////////////////////////////////////////
// LoadBitmaps
void CXAnalogClock::LoadBitmaps(CDC *pDC)
{
	if ((m_nFaceBitmapId == 0) || (m_nDateBitmapId == 0))
	{
		TRACE(_T("bitmap resource IDs have not been set\n"));
		return;
	}

	// load clockface.bmp
	if (!m_cdcClockFace.IsValid())
		VERIFY(m_cdcClockFace.LoadBitmap(m_nFaceBitmapId, pDC));

	// load date.bmp
	if (!m_cdcDate.IsValid())
		VERIFY(m_cdcDate.LoadBitmap(m_nDateBitmapId, pDC));

	if (!m_cdcPrevious.IsValid())
		VERIFY(m_cdcPrevious.LoadBitmap(m_nFaceBitmapId, pDC));
}

///////////////////////////////////////////////////////////////////////////////
//
static COLORREF IdealHandColor(COLORREF bg)
{
   const int nThreshold = 105;
   int crDelta = int(GetRValue(bg) * 0.299 + 
					 GetGValue(bg) * 0.587 + 
					 GetBValue(bg) * 0.114);
   return 255 - crDelta < nThreshold ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXAnalogClock::OnPaint()
{
	if ((m_nFaceBitmapId == 0) || (m_nDateBitmapId == 0))
	{
		TRACE(_T("bitmap resource IDs have not been set\n"));
		return;
	}

	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);

	LoadBitmaps(&dc);

	// system time may be disabled for some special purpose,
	// to allow display of specific time
	if (m_bUseSystemTime)
		m_time = CTime::GetCurrentTime();

	// this is used to adjust hour for different timezone
	if (m_nHourAdjust != 0)
	{
		CTimeSpan ts(0, abs(m_nHourAdjust), 0, 0);
		if (m_nHourAdjust >= 0)
			m_time += ts;
		else
			m_time -= ts;
	}


	if ((rect.Width() != m_cdcClockFace.m_sizeBitmap.cx) ||
		(rect.Height() != m_cdcClockFace.m_sizeBitmap.cy))
	{
		TRACE(_T("ERROR - control dimensions are incorrect\n"));
		ASSERT(FALSE);
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = memDC.SelectObject(&bitmap);

	// fill in background color
	CBrush brush(m_rgbBackground);
	memDC.FillRect(&rect, &brush);

#if 0  // -----------------------------------------------------------
	memDC.BitBlt(0, 0,
		rect.Width(), rect.Height(),
		&m_cdcPrevious.m_dcMemory,
		0, 0, SRCCOPY);
#endif // -----------------------------------------------------------

	// draw clock face
	::TransparentBlt(memDC.m_hDC,
		0, 0,
		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
		m_cdcClockFace.m_dcMemory.m_hDC,
		0, 0,
		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
		m_rgbTransparent);

	if (m_bShowDate)
		PaintDate(&memDC);

	COLORREF crMinuteHand = m_rgbMinuteHand;
	COLORREF crHourHand = m_rgbHourHand;
	COLORREF crScreenBackground = CLR_INVALID;

	if (m_bAutomaticHandColor)
	{
		// this clock face is transparent, so get the actual screen color,
		// to set up hand color
		CRect rectScreen;
		GetWindowRect(&rectScreen);
		HDC hScreenDC = ::GetDC(NULL);
		crScreenBackground = ::GetPixel(hScreenDC, 
			rectScreen.left+rect.Width()/2, rectScreen.top+rect.Height()/2);
		if (crScreenBackground == CLR_INVALID)
			crScreenBackground = m_rgbMinuteHand;
		::ReleaseDC(NULL, hScreenDC);
		TRACERGB(crScreenBackground);
		crMinuteHand = IdealHandColor(crScreenBackground);
		crHourHand = crMinuteHand;
	}

	PaintMinuteAndHourHands(&memDC, crMinuteHand, crHourHand, crScreenBackground);

	m_nPrevMinute = GetMinute();

	if (m_bShowSecondHand)
	{
		int nSecond = GetSecond();
		PaintSecondHand(&memDC, nSecond, m_rgbSecondHand, crScreenBackground);
	}

	// finally blt to screen
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	if (pOldBitmap)
		memDC.SelectObject(pOldBitmap);

	// Do not call CStatic::OnPaint() for painting messages
}

///////////////////////////////////////////////////////////////////////////////
// ConvertToDegrees
float CXAnalogClock::ConvertToDegrees(float dTime)
{
	float degrees = 0.;

	degrees = (float)(90. - dTime * 6.0);
	if (degrees < 0.0)
		degrees = (float)360. + degrees;
	return degrees;
}

///////////////////////////////////////////////////////////////////////////////
// PaintDate
void CXAnalogClock::PaintDate(CDC *pDC)
{
	LOGFONT lf;
	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = GetParent()->GetFont();
	if (pFont)
	{
		pFont->GetLogFont(&lf);
		_tcscpy(lf.lfFaceName, _T("Arial"));
#if 0  // -----------------------------------------------------------
		if (lf.lfHeight < 0)
			lf.lfHeight += 2;
		else
			lf.lfHeight -= 2;
#endif // -----------------------------------------------------------
		lf.lfQuality = ANTIALIASED_QUALITY;

		CFont font;
		font.CreateFontIndirect(&lf);

		CString strDate = _T("");
		strDate.Format(_T("%2d"), GetDay());

		CRect rect(0, 0, DATE_BOX_WIDTH, DATE_BOX_HEIGHT);

		// create temporary dc to hold the date numerals
		CDCBuffer cdcTemp;
		VERIFY(cdcTemp.LoadBitmap(m_nDateBitmapId, pDC));
		CFont *pOldFont = cdcTemp.m_dcMemory.SelectObject(&font);
		CBrush brush(RGB(255,0,255));
		cdcTemp.m_dcMemory.FillRect(&rect, &brush);
		cdcTemp.m_dcMemory.SetBkColor(RGB(255,0,255));
		cdcTemp.m_dcMemory.ExtTextOut(2, 1, 0, &rect, strDate, NULL);

		// draw date box template
		pDC->BitBlt(DATE_BOX_XPOS, DATE_BOX_YPOS, DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			&m_cdcDate.m_dcMemory, 0, 0, SRCCOPY);

		// overlay with date numerals
		::TransparentBlt(pDC->m_hDC,
			DATE_BOX_XPOS+1, DATE_BOX_YPOS-1,
			DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			cdcTemp.m_dcMemory.m_hDC,
			0, 0,
			DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			RGB(255,0,255));

		cdcTemp.m_dcMemory.SelectObject(pOldFont);
	}
}

///////////////////////////////////////////////////////////////////////////////
// PaintMinuteAndHourHands
void CXAnalogClock::PaintMinuteAndHourHands(CDC *pDC, 
											COLORREF crMinuteHand, 
											COLORREF crHourHand,
											COLORREF crScreenBackground)
{
	///////////////////////////////////////////////////////////////////////////
	// draw minute hand

	int minute = GetMinute();

	float degrees, radians, cosine, sine;
	short x1, y1, x2, y2;

	degrees = ConvertToDegrees((float)minute);
	radians = (float) ((PI * degrees)/180.);
	cosine = (float)cos(radians);
	sine = (float)sin(radians);

	x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
	y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
	x2 = (short)(5.0 * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
	y2 = (short)(-5.0 * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);

	if (((minute >= 10) && (minute <= 20)) || ((minute >= 40) && (minute <= 50)))
	{
		x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2, y2, crMinuteHand, crScreenBackground);
		x1 = (short)((MINUTE_HAND_LENGTH-4) * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)((-MINUTE_HAND_LENGTH+4) * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2, y2-2, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2-1, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2+1, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2+2, crMinuteHand, crScreenBackground);
	}
	else
	{
		x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2, y2, crMinuteHand, crScreenBackground);
		x1 = (short)((MINUTE_HAND_LENGTH-4) * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)((-MINUTE_HAND_LENGTH+4) * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2-2, y2, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2-1, y2, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2+1, y2, crMinuteHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2+2, y2, crMinuteHand, crScreenBackground);
	}

	///////////////////////////////////////////////////////////////////////////
	// draw hour hand
	int hour = GetHour();
	if (hour > 12)
		hour -= 12;
	hour = hour*5 + GetMinute()/12;
	degrees = ConvertToDegrees((float)hour);
	radians = (float) ((PI * degrees)/180.);
	cosine = (float)cos(radians);
	sine = (float)sin(radians);

	x1 = (short)(HOUR_HAND_LENGTH * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
	y1 = (short)(-HOUR_HAND_LENGTH * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
	x2 = (short)(5.0 * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
	y2 = (short)(-5.0 * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);

	if (((hour >= 10) && (hour <= 20)) || ((hour >= 40) && (hour <= 50)))
	{
		DrawWuLine(pDC, x1, y1, x2, y2, crHourHand, crScreenBackground);
		x1 = (short)((HOUR_HAND_LENGTH-4) * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)((-HOUR_HAND_LENGTH+4) * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2, y2-2, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2-1, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2+1, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2, y2+2, crHourHand, crScreenBackground);
	}
	else
	{
		DrawWuLine(pDC, x1, y1, x2, y2, crHourHand, crScreenBackground);
		x1 = (short)((HOUR_HAND_LENGTH-4) * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		y1 = (short)((-HOUR_HAND_LENGTH+4) * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		DrawWuLine(pDC, x1, y1, x2-2, y2, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2-1, y2, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2+1, y2, crHourHand, crScreenBackground);
		DrawWuLine(pDC, x1, y1, x2+2, y2, crHourHand, crScreenBackground);
	}
}

///////////////////////////////////////////////////////////////////////////////
// PaintSecondHand
void CXAnalogClock::PaintSecondHand(CDC *pDC, 
									int nSecond, 
									COLORREF rgbSecondHand, 
									COLORREF crScreenBackground)
{
	if (m_bShowSecondHand)
	{
		// draw second hand

		float degrees = ConvertToDegrees((float)nSecond);
		float radians = (float)((PI * degrees)/180.);
		float cosine = (float)cos(radians);
		float sine = (float)sin(radians);

		short x1 = (short)(SECOND_HAND_LENGTH * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		short y1 = (short)(-SECOND_HAND_LENGTH * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);
		short x2 = (short)(5.0 * cosine + (float)m_cdcClockFace.m_sizeBitmap.cx/2);
		short y2 = (short)(-5.0 * sine + (float)m_cdcClockFace.m_sizeBitmap.cy/2);

		DrawWuLine(pDC, x1, y1, x2, y2, rgbSecondHand, crScreenBackground);
		DrawWuLine(pDC, x1+1, y1, x2+1, y2, rgbSecondHand, crScreenBackground);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CXAnalogClock::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXAnalogClock::OnTimer(UINT nIDEvent)
{
	if (!m_bUseSystemTime)
	{
		CTimeSpan ts(0, 0, 0, 1);
		m_time = m_time + ts;
	}
	Invalidate(FALSE);
	CStatic::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// Run
void CXAnalogClock::Run()
{
	m_nPrevMinute = -1;
	SetTimer(1, 1000, NULL);
	Invalidate();
	m_bStopped = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Stop
void CXAnalogClock::Stop()
{
	KillTimer(1);
	m_bStopped = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetSecond
int CXAnalogClock::GetSecond()
{
	if (m_bDebug)
		return 36;
	return m_time.GetSecond();
}

///////////////////////////////////////////////////////////////////////////////
// GetMinute
int CXAnalogClock::GetMinute()
{
	if (m_bDebug)
		return 3;
	return m_time.GetMinute();
}

///////////////////////////////////////////////////////////////////////////////
// GetHour
int CXAnalogClock::GetHour()
{
	if (m_bDebug)
		return 2;
	int hour = m_time.GetHour();
	if (hour > 12)
		hour -= 12;
	return hour;
}

///////////////////////////////////////////////////////////////////////////////
// GetDay
int CXAnalogClock::GetDay()
{
	if (m_bDebug)
		return 24;
	int day = m_time.GetDay();
	return day;
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXAnalogClock::OnDestroy() 
{
	Stop();
	CStatic::OnDestroy();
}
