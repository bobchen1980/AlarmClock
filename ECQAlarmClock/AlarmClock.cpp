#include "stdafx.h"
#include <windows.h>
#include <crtdbg.h>
#include <process.h>
#include <mmsystem.h>
#include "alarmclock.h"

CAlarmClock::CAlarmClock(void)
: m_bAbort(FALSE)
, m_hThread(NULL)
, m_pCB(NULL)
, m_dwUser(0)
, m_bBusy(FALSE)
{
	Reset();
	m_hEventAbort = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CAlarmClock::~CAlarmClock(void)
{
	Abort();

	CloseHandle(m_hEventAbort);
}

// This is the clock's motor and gears. Keep track of time, handle all alarms,
// process alarm actions and schedules repeat alarms. When the proc exit's the object's state
// becomes inactive or un-armed.
unsigned CAlarmClock::CalendarProc(void * arg)
{
	CAlarmClock*	pThis = (CAlarmClock*) arg;
	FILETIME		localFt;
	SYSTEMTIME		localTime;
	UINT			sleepTimeMs;
	HANDLE			hEventInterval;	// Used for sleeping during interval period.
	BOOL			rc;
	UFT				u1, u2;
	HANDLE			hWaits[2];

	hEventInterval = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(hEventInterval != NULL);

	hWaits[0]		= hEventInterval;
	hWaits[1]		= pThis->m_hEventAbort;

	ResetEvent(pThis->m_hEventAbort);

	// Loop to check for time expiration
	while(!pThis->m_bAbort)
	{
		// Get local time
		GetLocalTime(&localTime);
		rc = SystemTimeToFileTime(&localTime, &localFt);
		_ASSERT(rc);

		// Compare local time with alarm time.
		if (CompareFileTime(&localFt, &pThis->m_AlarmTime) >= 0)
		{	// Alarm time has been reached. Handle the action.
			pThis->AlarmTriggered(pThis);

			{
				CCS Lock(&pThis->m_csAlarm);
				if (!ScheduleRepeat(pThis))
				{
					pThis->m_bAbort = TRUE;
					break;	// Kill the time loop.
				}
				else
				{	// Repeat is scheduled. Go redrive the alarm loop.
					continue;	// Go repeat using the new alarm time.
				}
			}
		}
		else
		{	// Need to prepare the interval timer for a period of time to sleep.

			// See if more than one second until the alarm is reached.
			u1.ft = localFt;
			u2.ll = u1.ll + ONE_SECOND;

			if (CompareFileTime(&pThis->m_AlarmTime, &u2.ft) < 1)
			{	// One second or less remaining before alarm.
				sleepTimeMs = 1;	// Select fine interval for precision.
			}
			else
			{	// More than one second remaining.
				sleepTimeMs = 1000;	// Select course interval.
			}
		}

		// We're to sleep here until the event is signalled.
		if (!pThis->m_bAbort)
		{
			MMRESULT result = timeSetEvent(sleepTimeMs, 1,
			(LPTIMECALLBACK) hEventInterval, 0, TIME_ONESHOT|TIME_CALLBACK_EVENT_SET);
			_ASSERT(result != NULL);

			// Sleep here until the timer interval ends.
			WaitForMultipleObjects(2, hWaits, FALSE, INFINITE);
			ResetEvent(hEventInterval);
		}
	}

	// Aborted or the repeat count ran out.
	pThis->ResetRepeats();

	CloseHandle(hEventInterval);

	pThis->m_bBusy = FALSE;

	//Beep(1000,100);

	return 0;
}

// Set alarm using FILETIME and Callback
BOOL CAlarmClock::SetAlarm(FILETIME Time, LPALARMCB pCB, DWORD dwUser)
{
	unsigned int	threadid;

	CCS	Lock(&m_csAlarm);

	if (IsArmed()) return FALSE;	// Busy

	m_AlarmTime	= Time;	// Save the desired wakeup time.
	m_pCB		= pCB;
	m_dwUser	= dwUser;
	m_bAbort	= FALSE;
	m_bBusy		= TRUE;

	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_hThread = (HANDLE) _beginthreadex(0, 0, CalendarProc, this, 0, &threadid);

	return TRUE;
}

// Set alarm using SYSTEMTIME and Callback
BOOL CAlarmClock::SetAlarm(SYSTEMTIME Time, LPALARMCB pCB, DWORD dwUser)
{
	FILETIME	ft;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(pCB != NULL);

	SystemTimeToFileTime(&Time, &ft);
	return SetAlarm(ft, pCB, dwUser);
}

BOOL CAlarmClock::SetAlarm(int DD, int HH, int MM, int SS, int MS, LPALARMCB pCB, DWORD dwUser)
{
	SYSTEMTIME	st;
	UFT			ft;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(pCB != NULL);

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft.ft);
	ft.ll		+= DD * ONE_DAY;
	ft.ll		+= HH * ONE_HOUR;
	ft.ll		+= MM * ONE_MINUTE;
	ft.ll		+= SS * ONE_SECOND;
	ft.ll		+= MS * ONE_MILLISECOND;

	return SetAlarm(ft.ft, pCB, dwUser);
}

// Set alarm using FILETIME and HWND message
BOOL CAlarmClock::SetAlarm(FILETIME Time, HWND hwndUser, DWORD dwUser)
{
	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hwndUser != NULL);

	m_hEventUser	= NULL;
	m_hwndUser		= hwndUser;
	m_dwUser		= dwUser;

	return SetAlarm(Time, (LPALARMCB) NULL, dwUser);
}

BOOL CAlarmClock::SetAlarm(int DD, int HH, int MM, int SS, int MS, HWND hwndUser, DWORD dwUser)
{
	SYSTEMTIME	st;
	UFT			ft;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hwndUser != NULL);

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft.ft);
	ft.ll		+= DD * ONE_DAY;
	ft.ll		+= HH * ONE_HOUR;
	ft.ll		+= MM * ONE_MINUTE;
	ft.ll		+= SS * ONE_SECOND;
	ft.ll		+= MS * ONE_MILLISECOND;

	return SetAlarm(ft.ft, hwndUser, dwUser);
}

BOOL CAlarmClock::SetAlarm(SYSTEMTIME Time, HWND hwndUser, DWORD dwUser)
{
	UFT			ft;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hwndUser != NULL);

	SystemTimeToFileTime(&Time, &ft.ft);

	return SetAlarm(ft.ft, hwndUser, dwUser);
}

BOOL CAlarmClock::SetAlarm(FILETIME Time, HANDLE hEvent, DWORD dwUser)
{
	unsigned int threadid;

	CCS	Lock(&m_csAlarm);

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hEvent != NULL);

	m_hEventUser	= hEvent;
	m_dwUser		= dwUser;
	m_AlarmTime		= Time;
	m_bAbort		= FALSE;
	m_bBusy			= TRUE;
	
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_hThread = (HANDLE) _beginthreadex(0, 0, CalendarProc, this, 0, &threadid);

	return TRUE;
}

BOOL CAlarmClock::SetAlarm(SYSTEMTIME Time, HANDLE hEvent, DWORD dwUser)
{
	UFT			ft;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hEvent != NULL);

	SystemTimeToFileTime(&Time, &ft.ft);

	return SetAlarm(ft.ft, hEvent, dwUser);
}

BOOL CAlarmClock::SetAlarm(int DD, int HH, int MM, int SS, int MS, HANDLE hEvent, DWORD dwUser)
{
	UFT			ft;
	SYSTEMTIME	st;

	if (IsArmed()) return FALSE;	// Busy
	_ASSERT(hEvent != NULL);

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft.ft);
	ft.ll		+= DD * ONE_DAY;
	ft.ll		+= HH * ONE_HOUR;
	ft.ll		+= MM * ONE_MINUTE;
	ft.ll		+= SS * ONE_SECOND;
	ft.ll		+= MS * ONE_MILLISECOND;

	return SetAlarm(ft.ft, hEvent, dwUser);
}

BOOL CAlarmClock::SetAlarmAndWait(FILETIME Time)
{
	HANDLE	hEvent;
	BOOL	rc;
	HANDLE	hWaits[2];

	if (IsArmed()) return FALSE;	// Busy

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(hEvent != NULL);

	rc = SetAlarm(Time, hEvent);
	_ASSERT(rc);

	hWaits[0]	= hEvent;
	hWaits[1]	= m_hEventAbort;

	ResetEvent(m_hEventAbort);

	// Wait here until the alarm is triggered or aborted.
	WaitForMultipleObjects(2, hWaits, FALSE, INFINITE);

	CloseHandle(hEvent);

	return !m_bAbort;	// TRUE = time expired, FALSE = aborted.
}

BOOL CAlarmClock::SetAlarmAndWait(SYSTEMTIME Time)
{
	UFT		ft;

	if (IsArmed()) return FALSE;	// Busy

	SystemTimeToFileTime(&Time, &ft.ft);

	return SetAlarmAndWait(ft.ft);
}

BOOL CAlarmClock::SetAlarmAndWait(int DD, int HH, int MM, int SS, int MS)
{
	UFT			ft;
	SYSTEMTIME	st;

	if (IsArmed()) return FALSE;	// Busy

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft.ft);
	ft.ll		+= DD * ONE_DAY;
	ft.ll		+= HH * ONE_HOUR;
	ft.ll		+= MM * ONE_MINUTE;
	ft.ll		+= SS * ONE_SECOND;
	ft.ll		+= MS * ONE_MILLISECOND;

	return SetAlarmAndWait(ft.ft);
}

BOOL CAlarmClock::Abort(void)
{
	CCS	Lock(&m_csAlarm);

	if (!IsArmed()) return FALSE;

	m_bAbort = TRUE;
	SetEvent(m_hEventAbort);

	if (m_hThread != NULL)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_bAbort = FALSE;	// Enables the object to re-arm

	return TRUE;
}

BOOL CAlarmClock::IsArmed(void)
{
	return (m_bBusy);
}

// Reset this object to its initial state.
void CAlarmClock::Reset(void)
{
	Abort();

	m_hEventUser	= NULL;
	m_hwndUser		= NULL;
	m_dwUser		= 0;
	m_pCB			= NULL;

	m_AlarmTime.dwHighDateTime	= 0;
	m_AlarmTime.dwLowDateTime	= 0;

	ResetRepeats();
}

// Return all repeat controls to their initial states.
void CAlarmClock::ResetRepeats(void)
{
	ZeroMemory(&m_RepeatParms, sizeof(REPEAT_PARMS));
}

// All expired alarms will eventually call this function to reflect the
// status back to the user.
void CAlarmClock::AlarmTriggered(CAlarmClock* pThis)
{
	BOOL rc;

	_ASSERT(pThis);

	if (pThis->m_hEventUser != NULL)
	{
		SetEvent(pThis->m_hEventUser);
	}
	else
	if (pThis->m_hwndUser != NULL)
	{
		rc = ::PostMessage(pThis->m_hwndUser, UWM_TIMER, (WPARAM) pThis, (LPARAM) pThis->m_dwUser);
		//_ASSERT(rc);	// Probably a bad HWND was passed to SetAlarm()
						// or the last message is still being processed.
	}
	else
	if (pThis->m_pCB != NULL)
	{
		pThis->m_pCB(pThis, pThis->m_dwUser);
	}
	else
	{
		//_ASSERT(0);	// For some reason, no trigger types were set. Look at SetAlarm().
	}

	// For testing, make noise at alarm point
	if (!pThis->m_bAbort)
	{
		//Beep(6000,20);
	}
}

// Validate and apply repeat parameters.
BOOL CAlarmClock::SetRepeat(REPEAT_PARMS Parms)
{
	int		i, count;
	BOOL	bInvalid	= FALSE;

	CCS	Lock(&m_csAlarm);

	switch (Parms.Type)
	{
		case Repeat_Off:
			break;

		case Repeat_Interval:
			if (Parms.nRepeatCount == 0 && !Parms.bRepeatForever) bInvalid = TRUE;
			if (Parms.dd > 365 || Parms.hh > 24 || Parms.mm > 60 || Parms.ss > 60 || Parms.ms > 999 ||
				(Parms.dd + Parms.hh + Parms.mm + Parms.ss + Parms.ms == 0) )
				bInvalid = TRUE;
			break;

		case Repeat_Monthly:
			if (Parms.nRepeatCount == 0 && !Parms.bRepeatForever) bInvalid = TRUE;
			if (!(Parms.RepeatDOM >= 1 && Parms.RepeatDOM <= 31))
				bInvalid = TRUE;
			break;

		case Repeat_Weekdays:
			if (Parms.nRepeatCount == 0 && !Parms.bRepeatForever) bInvalid = TRUE;
			// Make sure the array is valid boolean TRUE or FALSE only
			for (i = 0, count = 0; i < 7; i++)
			{
				if (!(Parms.RepeatWD[i] == TRUE || Parms.RepeatWD[i] == FALSE))
				{
					_ASSERT(0);	// Invalid data in array, not TRUE nor FALSE
					bInvalid	= TRUE;
				}
				if (Parms.RepeatWD[i])
					count++;
			}

			if (count == 0) bInvalid = TRUE;	// No weekdays were selected.

			break;
	}

	// If everything above checks out then save the parms.
	if (!bInvalid)
	{
		CCS	Lock(&m_csRepeats);

		m_RepeatParms = Parms;
		return TRUE;
	}
	else
	{
		//_ASSERT(0);		// Break here in order to look at Parms
		return FALSE;	// Invalid parameter(s) detected.
	}
}

// See if there's a need to repeat an alarm. If so, set up the new alarm time
// and return TRUE to tell the time loop to keep running. Else return FALSE
// if no more repeats which allows the time loop to quit.
BOOL CAlarmClock::ScheduleRepeat(CAlarmClock* pThis)
{
	UFT			u1;
	SYSTEMTIME	st;
	BOOL		bRepeatUntil;

	CCS	Lock(&pThis->m_csRepeats);

	_ASSERT(pThis);

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &u1.ft);

	// If there is a non-zero m_RepeatParms.tRepeatUntil then we will use expiration date and
	// ignore the other repeat-end-conditions.
	bRepeatUntil = pThis->m_RepeatParms.tRepeatUntil.ll != 0ll;

	if (pThis->m_RepeatParms.bRepeatForever || pThis->m_RepeatParms.nRepeatCount > 0)
	{
		switch (pThis->m_RepeatParms.Type)
		{
			case Repeat_Off:
				return FALSE;	// Prevent calendar from continuing.
				break;

			case Repeat_Interval:
				CalculateNewAlarmInterval(pThis);
				break;

			case Repeat_Monthly:
				CalculateNewAlarmDOM(pThis);
				break;

			case Repeat_Weekdays:
				CalculateNewAlarmDOW(pThis);
				break;
		}

		if (bRepeatUntil)
		{	// Handle repeat until epiration date.
			if (u1.ll < pThis->m_RepeatParms.tRepeatUntil.ll)
				return TRUE;	// Expiration time not reached yet
		}
		else
		{	// Handle repeat forever or counter.
			if (!pThis->m_RepeatParms.bRepeatForever) pThis->m_RepeatParms.nRepeatCount--;
			return TRUE;	// count not exhausted.
		}
	}

	return FALSE;	// Here if count exhausted or repeat switch off
}

void CAlarmClock::CalculateNewAlarmInterval(CAlarmClock* pThis)
{
	UFT		u1, u2;

	_ASSERT(pThis);

	u1.ll	=	pThis->m_RepeatParms.hh	* ONE_HOUR;
	u1.ll	+=	pThis->m_RepeatParms.mm	* ONE_MINUTE;
	u1.ll	+=	pThis->m_RepeatParms.ss	* ONE_SECOND;
	u1.ll	+=	pThis->m_RepeatParms.ms	* ONE_MILLISECOND;

	u2.ft	= pThis->m_AlarmTime;	// Get old alarm
	u2.ll	+= u1.ll;				// Add offset to make new alarm.
	pThis->m_AlarmTime = u2.ft;
}

void CAlarmClock::CalculateNewAlarmDays(CAlarmClock* pThis)
{
	UFT		u1, u2;

	_ASSERT(pThis);

	u1.ll	= pThis->m_RepeatParms.dd * ONE_DAY;
	u2.ft	= pThis->m_AlarmTime;
	u2.ll	+= u1.ll;			// Add new offset to original alarm.
	pThis->m_AlarmTime = u2.ft;
}

void CAlarmClock::CalculateNewAlarmDOW(CAlarmClock* pThis)
{
	SYSTEMTIME today;
	UFT			u1;
	int			i;				// Index to current DOW in table
	int			count;			// # days in week for loop
	int			todayWD;		// SYSTEMTIME's WD + 1 (wrapped)
	int			nDaysFromNow;	// Set to # days from now to set next alarm

	_ASSERT(pThis);

	// Get the current DOW from LocalTime.
	GetLocalTime(&today);
	todayWD = ((today.wDayOfWeek + 1) % 7);	// Increment DOW to tomorrow

	// Scan from tomorrow forward until we get a WD flag
	for (i = todayWD, count = 1;; count++, i++)
	{
		i %= 7;	// Wrap around the weekend
		if (pThis->m_RepeatParms.RepeatWD[i])	// If we found a weekday to set at
		{
			nDaysFromNow = count;
			break;
		}
	}

	// Now we know the count of days until the next alarm.
	// So update m_AlarmTime with the new time.

	u1.ft = pThis->m_AlarmTime;
	u1.ll += (nDaysFromNow * ONE_DAY);
	pThis->m_AlarmTime = u1.ft;
}

void CAlarmClock::CalculateNewAlarmDOM(CAlarmClock* pThis)
{
	UFT			testFT;	// Stores test alarm value.
	SYSTEMTIME	st;
	int			dom = pThis->m_RepeatParms.RepeatDOM;

	_ASSERT(pThis);

	testFT.ft = pThis->m_AlarmTime;	// Start the scan using the alarm time.
	testFT.ll += ONE_DAY;	// Start scan "tomorrow".

	// Now loop each day and check the DOM from the st.
	// If no matching DOM then keep scanning. It might've been Feb 29th.

	for (;;)
	{
		FileTimeToSystemTime(&testFT.ft, &st);	// Convert test time to get DOM
		if (st.wDay == dom)
		{	// Matching DOM. Keep this time as the new alarm.
			pThis->m_AlarmTime = testFT.ft;
			return;
		}

		// No matching DOM. Keep adding a day at a time until we find it.
		testFT.ll += ONE_DAY;
	}
}

FILETIME CAlarmClock::GetAlarmTime(void)
{
	return m_AlarmTime;
}

BOOL CAlarmClock::SetRepeat_Interval(long nCount, int nDays, int nHours, int nMinutes, int nSeconds, int nMillisoconds)
{
	REPEAT_PARMS		parms;

	ZeroMemory(&parms, sizeof parms);

	parms.Type			= Repeat_Interval;
	parms.nRepeatCount	= nCount;
	parms.dd			= nDays;
	parms.hh			= nHours;
	parms.mm			= nMinutes;
	parms.ss			= nSeconds;
	parms.ms			= nMillisoconds;

	parms.bRepeatForever = (nCount == 0) ? TRUE:FALSE;

	return SetRepeat(parms);
}

BOOL CAlarmClock::SetRepeat_Monthly(long nCount, int DayOfMonth)
{
	REPEAT_PARMS		parms;

	ZeroMemory(&parms, sizeof parms);

	parms.Type			= Repeat_Monthly;
	parms.nRepeatCount	= nCount;
	parms.RepeatDOM		= DayOfMonth;

	parms.bRepeatForever = (nCount == 0) ? TRUE:FALSE;

	return SetRepeat(parms);
}

BOOL CAlarmClock::SetRepeat_Weekdays(long nCount, BOOL* lpbDay[7])
{
	REPEAT_PARMS		parms;

	ZeroMemory(&parms, sizeof parms);

	parms.Type			= Repeat_Weekdays;
	parms.nRepeatCount	= nCount;

	for (int i = 0; i < 7; i++) parms.RepeatWD[i] = *lpbDay[i];

	parms.bRepeatForever = (nCount == 0) ? TRUE:FALSE;

	return SetRepeat(parms);
}

void CAlarmClock::SetRepeatUntil(FILETIME* lpDateTime)
{
	CCS	Lock(&m_csRepeats);

	m_RepeatParms.tRepeatUntil.ft	= *lpDateTime;
	m_RepeatParms.bRepeatForever	= FALSE;
	m_RepeatParms.nRepeatCount		= 0;
}

void CAlarmClock::SetRepeatUntil(SYSTEMTIME* lpDateTime)
{
	CCS	Lock(&m_csRepeats);

	SystemTimeToFileTime(lpDateTime, &m_RepeatParms.tRepeatUntil.ft);
	m_RepeatParms.bRepeatForever	= FALSE;
	m_RepeatParms.nRepeatCount		= 0;
}

void CAlarmClock::GetRepeatUntil(FILETIME* lpDateTime)
{
	*lpDateTime = m_RepeatParms.tRepeatUntil.ft;
}

void CAlarmClock::GetRepeatUntil(SYSTEMTIME* lpDateTime)
{
	FileTimeToSystemTime(&m_RepeatParms.tRepeatUntil.ft, lpDateTime);
}
