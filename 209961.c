static Jsi_RC MySqlErrorNoCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    int n = mysql_errno(jdb->db);
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)n);
    return JSI_OK;
}