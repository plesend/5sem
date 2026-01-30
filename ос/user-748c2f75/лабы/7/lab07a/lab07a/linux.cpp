#include <stdio.h>
#include <time.h>

int main()
{
    time_t t = time(NULL);

    struct tm local, utc;

    localtime_r(&t, &local);
    gmtime_r(&t, &utc);

    int tzHours =
        (local.tm_hour - utc.tm_hour);

    if (local.tm_yday > utc.tm_yday) tzHours += 24;
    if (local.tm_yday < utc.tm_yday) tzHours -= 24;

    printf("%04d-%02d-%02dT%02d:%02d:%02d%+03d\n",
        local.tm_year + 1900,
        local.tm_mon + 1,
        local.tm_mday,
        local.tm_hour,
        local.tm_min,
        local.tm_sec,
        tzHours
    );

    return 0;
}
