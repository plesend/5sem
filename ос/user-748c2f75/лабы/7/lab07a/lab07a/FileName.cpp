#include <windows.h>
#include <stdio.h>

int main() {
	SYSTEMTIME stutc, stlocal;
	FILETIME ftutc, ftlocal;

	GetSystemTime(&stutc);
	GetLocalTime(&stlocal);

	SystemTimeToFileTime(&stutc, &ftutc);
	SystemTimeToFileTime(&stlocal, &ftlocal);

	ULARGE_INTEGER utc, local;

	utc.LowPart = ftutc.dwLowDateTime;
	utc.HighPart = ftutc.dwHighDateTime;
	local.LowPart = ftlocal.dwLowDateTime;
	local.HighPart = ftlocal.dwHighDateTime;

	LONGLONG diffSeconds = (LONGLONG)(local.QuadPart - utc.QuadPart) / 10000000;

	int timezoneplus = (int)(diffSeconds / 3600);

	printf(
		"%04d-%02d-%02dT%02d:%02d:%02d%+03d\n", stlocal.wYear, stlocal.wMonth,
		stlocal.wDay, stlocal.wHour, stlocal.wMinute,
		stlocal.wSecond, timezoneplus
	);
}