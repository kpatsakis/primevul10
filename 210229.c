int Jsi_MapEntryDelete (Jsi_MapEntry *entryPtr){
    switch (jsi_GetListType(entryPtr)) {
        case JSI_MAP_HASH: return Jsi_HashEntryDelete((Jsi_HashEntry*)entryPtr);
        case JSI_MAP_TREE: return Jsi_TreeEntryDelete((Jsi_TreeEntry*)entryPtr);
        case JSI_MAP_LIST: {
            Jsi_ListEntry *lptr = (Jsi_ListEntry*)entryPtr;
            Jsi_ListPop(lptr->list, lptr);
            Jsi_ListEntryDelete(lptr);
            return 1;
        }
    }
    return JSI_OK;
}