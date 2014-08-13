#ifndef XTRACE_H
#define XTRACE_H

#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4996)		// disable bogus deprecation warning

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

#define XTRACE_SHOW_FULLPATH	FALSE	// FALSE = only show base name of file
#define XTRACE_SHOW_THREAD_ID	TRUE	// TRUE = include thread id in output
#define XTRACE_FILE				FALSE	// TRUE = output to file

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

class xtracing_output_debug_string
{
public:
	xtracing_output_debug_string(LPCTSTR lpszFile, int line) :
		m_file(lpszFile),
		m_line(line)
	{
	}

	void operator() (LPCTSTR lpszFormat, ...)
	{
		va_list va;
		va_start(va, lpszFormat);

		TCHAR buf1[BUFFER_SIZE];
		TCHAR buf2[BUFFER_SIZE];

		// add the __FILE__ and __LINE__ to the front
		TCHAR *cp = (LPTSTR) m_file;
		
		if (!XTRACE_SHOW_FULLPATH)
		{
			cp = (TCHAR *)_tcsrchr(m_file, _T('\\'));
			if (cp)
				cp++;
		}

		if (XTRACE_SHOW_THREAD_ID)
		{
			_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] %s"), 
						cp, m_line, GetCurrentThreadId(), lpszFormat);
			buf1[_countof(buf1)-1] = _T('\0');
		}
		else
		{
			_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : %s"), 
						cp, m_line, lpszFormat);
			buf1[_countof(buf1)-1] = _T('\0');
		}

		// format the message as requested
		_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va);
		buf2[_countof(buf2)-1] = _T('\0');

		va_end(va);

		if (XTRACE_FILE)
		{
			TCHAR szPathName[MAX_PATH*2] = { 0 };

			::GetModuleFileName(NULL, szPathName, sizeof(szPathName)/sizeof(TCHAR)-2);

			TCHAR *cp = _tcsrchr(szPathName, _T('\\'));
			if (cp != NULL)
				*(cp+1) = _T('\0');
			_tcscat(szPathName, _T("\\_trace.log"));
			HANDLE hFile = ::CreateFile(szPathName, GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwRC = ::SetFilePointer(hFile,		// handle to file
											  0,			// bytes to move pointer
											  NULL,			// bytes to move pointer
											  FILE_END);	// starting point

				if (dwRC != INVALID_SET_FILE_POINTER)
				{
					DWORD dwWritten = 0;
					::WriteFile(hFile,							// handle to file
								buf2,							// data buffer
								(DWORD)_tcslen(buf2)*sizeof(TCHAR),	// number of bytes to write
								&dwWritten,						// number of bytes written
								NULL);							// overlapped buffer
				}

				::CloseHandle(hFile);
			}
		}
		else
		{
			// write it out
			OutputDebugString(buf2);
		}
	}

private:
	LPCTSTR	m_file;
	int		m_line;
	enum	{ BUFFER_SIZE = 4096 };
};

class xtracing_entry_output_debug_string
{
public:
	xtracing_entry_output_debug_string(LPCTSTR lpszFile, int line) :
		m_file(lpszFile),
		m_line(line)
	{
	}

	~xtracing_entry_output_debug_string()
	{
		TCHAR buf3[BUFFER_SIZE*3];
		_stprintf(buf3, _T("======  exiting scope:  %s"), buf2);
		buf3[_countof(buf3)-1] = _T('\0');
		OutputDebugString(buf3);
	}

	void operator() (LPCTSTR lpszFormat, ...)
	{
		va_list va;
		va_start(va, lpszFormat);

		TCHAR buf1[BUFFER_SIZE];

		// add the __FILE__ and __LINE__ to the front
		TCHAR *cp = (LPTSTR) m_file;
		
		if (!XTRACE_SHOW_FULLPATH)
		{
			cp = (TCHAR *)_tcsrchr(m_file, _T('\\'));
			if (cp)
				cp++;
		}

		if (XTRACE_SHOW_THREAD_ID)
		{
			_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] ======  %s"), 
						cp, m_line, GetCurrentThreadId(), lpszFormat);
			buf1[_countof(buf1)-1] = _T('\0');
		}
		else
		{
			_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : ======  %s"), 
						cp, m_line, lpszFormat);
			buf1[_countof(buf1)-1] = _T('\0');
		}

		// format the message as requested
		_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va);
		buf2[_countof(buf2)-1] = _T('\0');

		va_end(va);

		if (XTRACE_FILE)
		{
			TCHAR szPathName[MAX_PATH*2] = { 0 };

			::GetModuleFileName(NULL, szPathName, sizeof(szPathName)/sizeof(TCHAR)-2);

			TCHAR *cp = _tcsrchr(szPathName, _T('\\'));
			if (cp != NULL)
				*(cp+1) = _T('\0');
			_tcscat(szPathName, _T("\\_trace.log"));
			HANDLE hFile = ::CreateFile(szPathName, GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwRC = ::SetFilePointer(hFile,		// handle to file
											  0,			// bytes to move pointer
											  NULL,			// bytes to move pointer
											  FILE_END);	// starting point

				if (dwRC != INVALID_SET_FILE_POINTER)
				{
					DWORD dwWritten = 0;
					::WriteFile(hFile,							// handle to file
								buf2,							// data buffer
								(DWORD)_tcslen(buf2)*sizeof(TCHAR),	// number of bytes to write
								&dwWritten,						// number of bytes written
								NULL);							// overlapped buffer
				}

				::CloseHandle(hFile);
			}
		}
		else
		{
			// write it out
			OutputDebugString(buf2);
		}
	}

private:
	LPCTSTR	m_file;
	int		m_line;
	enum	{ BUFFER_SIZE = 4096 };
	TCHAR	buf2[BUFFER_SIZE*2];
};

#undef TRACE
#undef TRACE0
#undef TRACE1
#undef TRACE2
#undef TRACERECT
#undef TRACERGB
#undef TRACEPOINT
#undef TRACESIZE
#undef TRACEHILO

#define _DEBUGnow

// change following line to _DEBUGnow to output trace in release mode
#ifdef _DEBUG

#define TRACE (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACEENTRY (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACEERROR (xtracing_output_debug_string(_T(__FILE__), __LINE__ ))
#define TRACE0 TRACE
#define TRACE1 TRACE
#define TRACE2 TRACE

#define TRACEHILO(d) \
			WORD ___hi = HIWORD(d); WORD ___lo = LOWORD(d); \
			TRACE(_T(#d) _T(":  HIWORD = %u  LOWORD = %u\n"), ___hi, ___lo)

#define TRACESIZE(s) TRACE(_T(#s) _T(":  cx = %d  cy = %d\n"), \
                           (s).cx, (s).cy)

#define TRACEPOINT(p) TRACE(_T(#p) _T(":  x = %d  y = %d\n"), \
                           (p).x, (p).y)

#define TRACERECT(r) TRACE(_T(#r) _T(":  left = %d  top = %d  right = %d  bottom = %d\n"), \
                           (r).left, (r).top, (r).right, (r).bottom)

#define TRACERGB(cr) TRACE(_T(#cr) _T(":  RGB(%d,%d,%d)\n"), \
                           GetRValue(cr), GetGValue(cr), GetBValue(cr));

#else

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#define TRACE		__noop
#define TRACEERROR	__noop
#define TRACE0		__noop
#define TRACE1		__noop
#define TRACE2		__noop
#define TRACERECT	__noop
#define TRACERGB	__noop
#define TRACEPOINT	__noop
#define TRACESIZE	__noop
#define TRACEHILO	__noop

#endif	//_DEBUG

#pragma warning(pop)

#endif //XTRACE_H
