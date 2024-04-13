Jsi_RC Jsi_MapKeysDump(Jsi_Interp *interp, Jsi_Map *mapPtr, Jsi_Value **ret, int flags){
    SIGASSERT(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return Jsi_HashKeysDump(interp, mapPtr->v.hash, ret, flags);
        case JSI_MAP_TREE: return Jsi_TreeKeysDump(interp, mapPtr->v.tree, ret, flags);
        case JSI_MAP_LIST: break; // TODO: dump numbers?
        case JSI_MAP_NONE: break;
    }
    return JSI_ERROR;
}