void* Jsi_MapKeyGet(Jsi_MapEntry *h, int flags){
    switch (jsi_GetListType(h)) {
        case JSI_MAP_HASH: return Jsi_HashKeyGet((Jsi_HashEntry*)h);
        case JSI_MAP_TREE: return Jsi_TreeKeyGet((Jsi_TreeEntry*)h);
        case JSI_MAP_LIST: break;
        case JSI_MAP_NONE: break;
    }
    return NULL;
}