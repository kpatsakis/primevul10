static Jsi_RC MySqlResetCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
#if (MYSQL_VERSION_ID >= 50703 && !defined(JSI_NO_MYSQL_RESET))
    int oldMax = jdb->maxStmts;
    jdb->maxStmts = 0;
    mdbStmtLimit(jdb);
    jdb->maxStmts = oldMax;
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)mysql_reset_connection(jdb->db));
#else
    Jsi_LogWarn("mysql reset unavailable: requires version 5.7.3+");
#endif
    return JSI_OK;    
}