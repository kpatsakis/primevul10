void Jsi_EventuallyFree(Jsi_Interp* interp, void *data, Jsi_DeleteProc* proc) {
    Jsi_HashEntry *hPtr = Jsi_HashEntryFind(interp->preserveTbl, data);
    if (!hPtr) {
        (*proc)(interp, data);
        return;
    }
    PreserveData *ptr = (PreserveData*)Jsi_HashValueGet(hPtr);
    assert(ptr && ptr->interp == interp);
    JSI_NOWARN(ptr);
    Jsi_HashEntryDelete(hPtr);
}