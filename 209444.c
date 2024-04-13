static Jsi_RC IterGetKeysCallback(Jsi_Tree* tree, Jsi_TreeEntry *hPtr, void *data)
{
    Jsi_IterObj *io = (Jsi_IterObj *)data;
    if (!hPtr->f.bits.dontenum) {
        IterObjInsert(io, hPtr);
    }
    return JSI_OK;
}