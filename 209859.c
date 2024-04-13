Jsi_HashSearchFirst(Jsi_Hash *tablePtr, Jsi_HashSearch *searchPtr) 
{
    searchPtr->tablePtr = tablePtr;
    searchPtr->nextIndex = 0;
    searchPtr->nextEntryPtr = NULL;
    return Jsi_HashSearchNext(searchPtr);
}