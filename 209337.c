static MySqlObj *_mysql_getDbHandle(Jsi_Interp *interp, Jsi_Value *_this, Jsi_Func *funcPtr)
{
    MySqlObj *jdb = (MySqlObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!jdb) {
        Jsi_LogError("MySql call to a non-mysql object");
        return NULL;
    }
    if (!jdb->db)
    {
        Jsi_LogError("MySql db is closed");
        return NULL;
    }
    return jdb;
}