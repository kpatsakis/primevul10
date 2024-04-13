Jsi_List *Jsi_ListNew(Jsi_Interp *interp, Jsi_Wide flags, Jsi_HashDeleteProc *freeProc)
{
    Jsi_List *list = (Jsi_List *)Jsi_Calloc(1, sizeof(Jsi_List));
    list->sig = JSI_SIG_LIST;
    list->opts.flags = flags;
    list->opts.freeHashProc = freeProc;
    list->opts.interp = interp;
    list->opts.mapType = JSI_MAP_LIST;
    list->opts.keyType = (Jsi_Key_Type)-1;
    return list;
}