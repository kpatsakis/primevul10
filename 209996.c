void* Jsi_MapValueGet(Jsi_MapEntry *h){
    switch (jsi_GetListType(h)) {
        case JSI_MAP_HASH: return Jsi_HashValueGet((Jsi_HashEntry*)h);
        case JSI_MAP_TREE: return Jsi_TreeValueGet((Jsi_TreeEntry*)h);
        case JSI_MAP_LIST: return Jsi_ListValueGet((Jsi_ListEntry*)h);
        case JSI_MAP_NONE: break;
    }
    return NULL;
}