Jsi_ListEntry *Jsi_ListEntryNew(Jsi_List* list, const void *value, Jsi_ListEntry *before) {
    SIGASSERT(list, LIST);
    Jsi_ListEntry *l = (Jsi_ListEntry*)Jsi_Calloc(1, sizeof(Jsi_ListEntry));
    l->sig = JSI_SIG_LISTENTRY;
    l->typ = JSI_MAP_LIST;
    l->list = list;
    l->value = (void*)value;
    Jsi_ListPush(list, l, before);
    return l;
}