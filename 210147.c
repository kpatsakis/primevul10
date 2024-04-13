Jsi_ListEntry* Jsi_ListSearchFirst (Jsi_List *list, Jsi_ListSearch *searchPtr, int flags)
{
    SIGASSERT(list, LIST);
    searchPtr->flags = flags;
    Jsi_ListEntry *lptr;
    if (flags & JSI_LIST_REVERSE) {
        lptr = Jsi_ListGetBack(list);
        searchPtr->nextEntryPtr = (lptr?Jsi_ListEntryPrev(lptr):NULL);
    } else {
        lptr = Jsi_ListGetFront(list);
        searchPtr->nextEntryPtr = (lptr?Jsi_ListEntryNext(lptr):NULL);
    }
    return lptr;
}