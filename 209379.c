void Jsi_Release(Jsi_Interp* interp, void *data) {
    Jsi_HashEntry *hPtr = Jsi_HashEntryFind(interp->preserveTbl, data);
    if (!hPtr) return;
    PreserveData *ptr = (PreserveData*)Jsi_HashValueGet(hPtr);
    if (!ptr) return;
    assert(ptr->interp == interp);
    if (--ptr->refCnt > 0) return;
    if (ptr->proc)
        (*ptr->proc)(interp, data);
    Jsi_Free(ptr);
    Jsi_HashEntryDelete(hPtr);
}