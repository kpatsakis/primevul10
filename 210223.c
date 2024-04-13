static void mdbJsToMyTime(Jsi_Number time, MYSQL_TIME* mtm, int utc)
{
    struct tm tm;
    time_t tim = (time_t)(time/1000);
    if (utc)
        gmtime_r(&tim, &tm);
    else
        localtime_r(&tim, &tm);
    mtm->second = tm.tm_sec;
    mtm->minute = tm.tm_min;
    mtm->hour = tm.tm_hour;
    mtm->day = tm.tm_mday;
    mtm->month = tm.tm_mon+1;
    mtm->year = tm.tm_year + 1900;
    Jsi_Number secs = (tim/1000.0);
    mtm->second_part = (int)((secs-(int)secs)*1000000);
}