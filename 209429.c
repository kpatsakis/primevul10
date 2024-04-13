uint Jsi_ListSize(Jsi_List *list) {
    SIGASSERT(list, LIST);
    return list->numEntries;
}