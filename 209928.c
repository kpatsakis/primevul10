void mdbTypeNameHashInit(MySqlObj *jdb) {
    Jsi_Interp *interp = jdb->interp;
    Jsi_Hash *hPtr = jdb->typeNameHash = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    Jsi_HashSet(hPtr, (void*)"string", (void*)MYSQL_TYPE_STRING);
    Jsi_HashSet(hPtr, (void*)"double", (void*)MYSQL_TYPE_DOUBLE);
    Jsi_HashSet(hPtr, (void*)"integer", (void*)MYSQL_TYPE_LONGLONG);
    Jsi_HashSet(hPtr, (void*)"bool", (void*)MYSQL_TYPE_TINY);
    Jsi_HashSet(hPtr, (void*)"blob", (void*)MYSQL_TYPE_BLOB);
    Jsi_HashSet(hPtr, (void*)"date", (void*)MYSQL_TYPE_DATE);
    Jsi_HashSet(hPtr, (void*)"time", (void*)MYSQL_TYPE_TIME);
    Jsi_HashSet(hPtr, (void*)"timestamp", (void*)MYSQL_TYPE_TIMESTAMP);
    Jsi_HashSet(hPtr, (void*)"datetime", (void*)MYSQL_TYPE_DATETIME);
}