Jsi_ListEntry* Jsi_ListSearchNext (Jsi_ListSearch *searchPtr)
{
    Jsi_ListEntry *lptr = searchPtr->nextEntryPtr;
    searchPtr->nextEntryPtr = (lptr?(searchPtr->flags & JSI_LIST_REVERSE ? Jsi_ListEntryPrev(lptr): Jsi_ListEntryNext(lptr)):NULL);
    return lptr;
}