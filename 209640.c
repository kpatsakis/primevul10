static Jsi_RC MySqlErrorStateCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    const char *str = mysql_sqlstate(jdb->db);
    if (str)
        Jsi_ValueMakeStringDup(interp, ret, str);
    return JSI_OK;
}