Jsi_Map* Jsi_MapNew (Jsi_Interp *interp, Jsi_Map_Type listType, Jsi_Key_Type keyType, Jsi_MapDeleteProc *freeProc)
{
    Jsi_Map *lPtr, lval = {.sig=JSI_SIG_MAP};
    lval.typ = listType;
    switch (listType) {
        case JSI_MAP_HASH: lval.v.hash = Jsi_HashNew(interp, keyType, (Jsi_HashDeleteProc*)freeProc); break;
        case JSI_MAP_TREE: lval.v.tree = Jsi_TreeNew(interp, keyType, (Jsi_TreeDeleteProc*)freeProc); break;
        case JSI_MAP_LIST: lval.v.list = Jsi_ListNew(interp, keyType, (Jsi_HashDeleteProc*)freeProc); break;
        default: return NULL;
    }
    if (!lval.v.hash) return NULL;
    lPtr = (Jsi_Map*)Jsi_Malloc(sizeof(*lPtr));
    *lPtr = lval;
    return lPtr;
}