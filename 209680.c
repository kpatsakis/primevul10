void jsi_DelAssocData(Jsi_Interp *interp, void *data) {
    AssocData *ptr = (AssocData *)data;
    if (!ptr) return;
    if (ptr->delProc)
        ptr->delProc(interp, ptr->data);
    Jsi_Free(ptr);
}