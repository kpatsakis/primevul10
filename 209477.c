static Jsi_RC MySqlLastRowidCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Wide rowid;
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    rowid = mdbLastInsertRowid(jdb);
    Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)rowid);
    return JSI_OK;
}