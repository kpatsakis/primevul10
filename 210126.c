static Jsi_RC MySqlReconnectCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                         Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb  = _mysql_getDbHandle(interp, _this, funcPtr);
    if (!jdb) return JSI_ERROR;
    int oldMax = jdb->maxStmts;
    jdb->maxStmts = 0;
    mdbStmtLimit(jdb);
    jdb->maxStmts = oldMax;
    mysql_close(jdb->db);
    jdb->db = mysql_init(NULL);
    if (!mdbConnect(interp, jdb)) 
        return Jsi_LogError("reconnect failed: %s", mysql_error(jdb->db));
    return JSI_OK;
}