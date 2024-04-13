static Jsi_RC mysqlObjFree(Jsi_Interp *interp, void *data)
{
    MySqlObj *jdb = (MySqlObj*)data;
    SQLSIGASSERT(jdb,DB);
    jdb->_->activeCnt--;
    mysqlObjErase(jdb);
    _JSI_MEMCLEAR(jdb);
    Jsi_Free(jdb);
    return JSI_OK;
}