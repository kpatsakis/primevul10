void dbTypeNameHashInit(Jsi_Db *jdb) {
    Jsi_Interp *interp = jdb->interp;
    Jsi_Hash *hPtr = jdb->typeNameHash = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    Jsi_HashSet(hPtr, (void*)"blob", (void*)JSI_OPTION_STRBUF);
    Jsi_HashSet(hPtr, (void*)"string", (void*)JSI_OPTION_STRING);
    Jsi_HashSet(hPtr, (void*)"double", (void*)JSI_OPTION_DOUBLE);
    Jsi_HashSet(hPtr, (void*)"integer", (void*)JSI_OPTION_INT64);
    Jsi_HashSet(hPtr, (void*)"bool", (void*)JSI_OPTION_BOOL);
    Jsi_HashSet(hPtr, (void*)"time_d", (void*)JSI_OPTION_TIME_D);
    Jsi_HashSet(hPtr, (void*)"time_w", (void*)JSI_OPTION_TIME_W);
    Jsi_HashSet(hPtr, (void*)"time_t", (void*)JSI_OPTION_TIME_T);
    Jsi_HashSet(hPtr, (void*)"date", (void*)JSI_OPTION_TIME_W);
    Jsi_HashSet(hPtr, (void*)"time", (void*)JSI_OPTION_TIME_W);
    Jsi_HashSet(hPtr, (void*)"datetime", (void*)JSI_OPTION_TIME_W);
}