static Jsi_RC MySqlEvalCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                         Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb  = _mysql_getDbHandle(interp, _this, funcPtr);
    if (!jdb) return JSI_ERROR;
    int zLen, cnt = 0;
    Jsi_RC rc = JSI_OK;
    const char *zSql = Jsi_ValueArrayIndexToStr(interp, args, 0, &zLen);

    if (mysql_real_query(jdb->db, zSql, zLen))
        rc = Jsi_LogError("mysql error: %s", mysql_error(jdb->db));
    else if (jdb->enableMulti) {
        MYSQL_RES *results;
        int sr = mysql_next_result(jdb->db);
        while (sr == 0 && (results = mysql_store_result(jdb->db)))
            mysql_free_result(results);
        cnt = mysql_field_count(jdb->db);
    }
        
    if (rc == JSI_OK)
        Jsi_ValueMakeNumber(interp, ret, (Jsi_Number)cnt);
    return rc;
}