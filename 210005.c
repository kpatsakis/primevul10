static Jsi_RC MySqlInfoCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    MySqlObj *jdb;
    if (!(jdb = _mysql_getDbHandle(interp, _this, funcPtr))) return JSI_ERROR;
    Jsi_Obj *nobj = Jsi_ObjNew(interp);
    Jsi_ValueMakeObject(interp, ret, nobj);
    const char *str, *svals[20];
    int i = 0;
    svals[i++] = "clientInfo";
    svals[i++] = mysql_get_client_info();
    svals[i++] = "hostInfo";
    svals[i++] = mysql_get_host_info(jdb->db);
    svals[i++] = "serverInfo";
    svals[i++] = mysql_get_server_info(jdb->db);
    svals[i++] = "stat";
    svals[i++] = mysql_stat(jdb->db);
    svals[i++] = 0;
    i = 0;
    while (svals[i]) {
        str = svals[i+1];
        Jsi_ObjInsert(interp, nobj, svals[i], str?Jsi_ValueNewStringDup(interp, str):Jsi_ValueNewNull(interp), 0);
    }
    Jsi_ObjInsert(interp, nobj, "threadId", Jsi_ValueNewNumber(interp, (Jsi_Number)mysql_thread_id(jdb->db)), 0);
    Jsi_ObjInsert(interp, nobj, "protocolVersion", Jsi_ValueNewNumber(interp, (Jsi_Number)mysql_get_proto_info(jdb->db)), 0);
    Jsi_ObjInsert(interp, nobj, "clientVersion", Jsi_ValueNewNumber(interp, (Jsi_Number)mysql_get_client_version()), 0);
    Jsi_ObjInsert(interp, nobj, "serverVersion", Jsi_ValueNewNumber(interp, (Jsi_Number)mysql_get_server_version(jdb->db)), 0);
    Jsi_ObjInsert(interp, nobj, "warningCount", Jsi_ValueNewNumber(interp, (Jsi_Number)mysql_warning_count(jdb->db)), 0);
    return JSI_OK;
}