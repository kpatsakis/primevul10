static Jsi_RC sqliteObjFree(Jsi_Interp *interp, void *data)
{
    Jsi_Db *db = (Jsi_Db*)data;
    SQLSIGASSERT(db,DB);
    db->_->activeCnt--;
    sqliteObjErase(db);
    _JSI_MEMCLEAR(db);
    Jsi_Free(db);
    return JSI_OK;
}