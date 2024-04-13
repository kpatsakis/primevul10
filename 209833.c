void Jsi_InterpOnDelete(Jsi_Interp *interp, Jsi_DeleteProc *freeProc, void *ptr)
{
    if (freeProc)
        Jsi_HashSet(interp->onDeleteTbl, ptr, (void*)freeProc);
    else {
        Jsi_HashEntry *hPtr = Jsi_HashEntryFind(interp->onDeleteTbl, ptr);
        if (hPtr)
            Jsi_HashEntryDelete(hPtr);
    }
}