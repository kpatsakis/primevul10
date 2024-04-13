static Jsi_RC MySqlPingCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    int n = mysql_ping(jdb->db);
    bool noErr = 0;
    Jsi_Value *val = Jsi_ValueArrayIndex(interp, args, 0);
    if (val)
        Jsi_ValueGetBoolean(interp, val, &noErr);
    if (n && noErr==0) 
        return Jsi_LogError("ping failed: (%d) %s", n, mysql_error(jdb->db));
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)n);

    return JSI_OK;    
}