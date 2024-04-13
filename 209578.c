static void jsiSqlFuncUnixTime(sqlite3_context *context, int argc, sqlite3_value**argv) {
    Jsi_Db *jdb = (Jsi_Db*)sqlite3_user_data(context);
    SQLSIGASSERT(jdb,DB);
    Jsi_Interp *interp = jdb->interp;
    if (argc>3) {
        Jsi_LogWarn("sqlite unixtime, expected: str fmt isutc");
        return;
    }
    const char *str = NULL, *fmt = NULL;
    bool isUtc = 0;
    if (argc>=1)
        str = (char *)sqlite3_value_text(argv[0]);
    if (argc>=2)
        fmt = (char *)sqlite3_value_text(argv[1]);
    if (argc>=3)
        isUtc = (bool)sqlite3_value_int64(argv[2]);
    Jsi_Number d = 0;
    Jsi_DatetimeParse(interp, str, fmt, isUtc, &d, 0);
    sqlite3_result_double(context, (double)d);
}