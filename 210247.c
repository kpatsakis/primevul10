void Jsi_TreeSearchDone(Jsi_TreeSearch *searchPtr)
{
    if (searchPtr->Ptrs != searchPtr->staticPtrs)
        Jsi_Free(searchPtr->Ptrs);
    searchPtr->Ptrs = searchPtr->staticPtrs;
    searchPtr->top = 0;
}