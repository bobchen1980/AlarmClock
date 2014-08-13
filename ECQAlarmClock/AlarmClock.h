#pragma once

#define ONE_MILLISECOND	(10000ll)
#define ONE_SECOND		(10000000ll)
#define ONE_MINUTE		(ONE_SECOND	*	60)
#define ONE_HOUR		(ONE_SECOND	*	3600)
#define ONE_DAY			(ONE_HOUR	*	24)
#define ONE_WEEK		(ONE_DAY	*	7)

#define UWM_TIMER		(WM_USER+1)			// Modify the message id if it interferes
											// with other WM_USER messages

// Critical section wrapper class
class CSection : public CRITICAL_SECTION
{
public:
	__forceinline CSection()
	{
		InitializeCriticalSection(this);
	};

	__forceinline ~CSection()
	{
		DeleteCriticalSection(this);
	};
};

class CCS
{
public:
	// Constructor to lock the CS
	__forceinline CCS(CSection* lpCS)
		: m_lpCS(NULL)
	{
		_ASSERT(lpCS != NULL);

		m_lpCS = lpCS;	// Save
		EnterCriticalSection(lpCS);
	}

	// Destructor unlocks it.
	__forceinline ~CCS()
	{
		if (m_lpCS != NULL)
			LeaveCriticalSection(m_lpCS);
	}
	
	CSection*	m_lpCS;
};

class CAlarmClock;
typedef BOOL (__stdcall *LPALARMCB)(CAlarmClock* pThis, DWORD dwUserData);

typedef union	_uft	// Used to compute time offsets
{
	FILETIME	ft;
	ULONGLONG	ll;
}	UFT;

typedef enum eRepeatType
{
	Repeat_Off	= 0,	// Turn all repeats off
	Repeat_Interval,	// Repeat every DD:HH:MM:SS:MS
	Repeat_Monthly,		// Day every month
	Repeat_Weekdays		// Certain days of the week
}	REPEAT_TYPE;

typedef struct _repeat_parms
{
	REPEAT_TYPE			Type;			// One of several types.

	// The following repeat types are mutually exclusive
	int					dd, hh, mm, ss, ms;	// Repeat interval (dd:hh:mm:ss:ms)
	BOOL				RepeatWD[7];	// Day of week ([0] = sun, [1] = mon, [2] = tue, etc.
	int					RepeatDOM;		// Day of month (0 = off, 1 to n

	// Repeat control.
	// If tRepeatUntil != 0, ignore nRepeatcount and bRepeatForever.
	DWORD				nRepeatCount;	// 0 to n times.
	BOOL				bRepeatForever;	// Don't ever stop repeating
	UFT					tRepeatUntil;	// Date / Time to stop repeating. (UFT.ft = filetime)
}	REPEAT_PARMS;




class CAlarmClock
{
public:
						CAlarmClock(void);
						~CAlarmClock(void);

						// Set the alarm and start the clock. This group uses a callback
						// to alert the caller when the alarm time is reached.
	BOOL				SetAlarm(FILETIME Time, LPALARMCB pCB, DWORD dwUser = 0);
	BOOL				SetAlarm(SYSTEMTIME Time, LPALARMCB pCB, DWORD dwUser = 0);
	BOOL				SetAlarm(int DD, int HH, int MM, int SS, int MS, LPALARMCB pCB, DWORD dwUser = 0);

						// Set the alarm and start the clock. This group uses a window message
						// to alert the caller when the alarm time is reached.
	BOOL				SetAlarm(FILETIME Time, HWND hwndUser, DWORD dwUser = 0);
	BOOL				SetAlarm(SYSTEMTIME Time, HWND hwndUser, DWORD dwUser = 0);
	BOOL				SetAlarm(int DD, int HH, int MM, int SS, int MS, HWND hwndUser, DWORD dwUser = 0);

						// Set the alarm and start the clock. This group uses an event
						// to alert the caller when the alarm time is reached.
	BOOL				SetAlarm(FILETIME Time, HANDLE hEvent, DWORD dwUser = 0);
	BOOL				SetAlarm(SYSTEMTIME Time, HANDLE hEvent, DWORD dwUser = 0);
	BOOL				SetAlarm(int DD, int HH, int MM, int SS, int MS, HANDLE hEvent, DWORD dwUser = 0);

						// Set the alarm and start the clock. Wait here blocked until the
						// alarm time is reached. Obviously repeat alarms aren't supported here.
	BOOL				SetAlarmAndWait(FILETIME Time);
	BOOL				SetAlarmAndWait(SYSTEMTIME Time);
	BOOL				SetAlarmAndWait(int DD, int HH, int MM, int SS, int MS);

	BOOL				Abort(void);
	BOOL				IsArmed(void);
	void				Reset(void);
	BOOL				SetRepeat(REPEAT_PARMS Parms);
	BOOL				SetRepeat_Interval(long nCount, int nDays, int nHours, int nMinutes, int nSeconds, int nMillisoconds);
	BOOL				SetRepeat_Monthly(long nCount, int DayOfMonth);
	BOOL				SetRepeat_Weekdays(long nCount, BOOL* lpbDay[7]);
	void				SetRepeatUntil(FILETIME* lpDateTime);
	void				SetRepeatUntil(SYSTEMTIME* lpDateTime);
	void				GetRepeatUntil(FILETIME* lpDateTime);
	void				GetRepeatUntil(SYSTEMTIME* lpDateTime);
	void				ResetRepeats(void);	// Turn off and reinit repeat settings.

	FILETIME			GetAlarmTime(void);

protected:
	FILETIME			m_AlarmTime;		// Wakeup date/time
	LPALARMCB			m_pCB;				// Caller's alarm callback
	DWORD				m_dwUser;			// User's data for callback
	HWND				m_hwndUser;			// Callers' hwnd for message alerts
	HANDLE				m_hEventUser;		// User's event handle for triggers
	HANDLE				m_hEventAbort;		// Kills any pending wait periods during abort.

	CSection			m_csRepeats;		// Protect access to this struct.
	REPEAT_PARMS		m_RepeatParms;		// Where the repeat info is kept.

	CSection			m_csAlarm;			// Must hold b4 accessing following vars
	BOOL				m_bAbort;			// Tells worker to close
	BOOL				m_bBusy;			// Thread proc is busy measuring time.
	HANDLE				m_hThread;			// Worker thread handle

	// Worker thread and its helper (static) functions.
	//static void _cdecl	CalendarProc( void * arg);
	static unsigned __stdcall CalendarProc(void * arg);
	static void			AlarmTriggered(CAlarmClock* pThis);
	static BOOL			ScheduleRepeat(CAlarmClock* pThis);
	static void			CalculateNewAlarmInterval(CAlarmClock* pThis);
	static void			CalculateNewAlarmDays(CAlarmClock* pThis);
	static void			CalculateNewAlarmDOW(CAlarmClock* pThis);
	static void			CalculateNewAlarmDOM(CAlarmClock* pThis);
};
