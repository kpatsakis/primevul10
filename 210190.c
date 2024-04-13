static Jsi_Number mdbMyTimeToJS(MYSQL_TIME* mtm)
{
    struct tm tm;
    tm.tm_sec = mtm->second;
    tm.tm_min = mtm->minute;
    tm.tm_hour = mtm->hour;
    tm.tm_mday = mtm->day;
    tm.tm_mon = mtm->month-1;
    tm.tm_year = mtm->year - 1900;
    time_t tim = mktime(&tm);
    if (tim == (time_t)-1)
        return -1;
    return (Jsi_Number)tim*1000 + (Jsi_Number)mtm->second_part/1000000.0;
}