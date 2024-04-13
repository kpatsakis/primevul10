static Jsi_RC MySqlAffectedRowsCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)mysql_affected_rows(jdb->db));
    return JSI_OK;
}