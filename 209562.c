void Jsi_Preserve(Jsi_Interp* interp, void *data) {
    bool isNew;
    PreserveData *ptr;
    Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->preserveTbl, data, &isNew);
    assert(hPtr);
    if (!isNew) {
        ptr = (PreserveData*)Jsi_HashValueGet(hPtr);
        if (ptr) {
            assert(interp == ptr->interp);
            ptr->refCnt++;
        }
    } else {
        ptr = (PreserveData*)Jsi_Calloc(1,sizeof(*ptr));
        Jsi_HashValueSet(hPtr, ptr);
        ptr->interp = interp;
        ptr->data = data;
        ptr->refCnt = 1;
    }
}